#include "dsp_effects.h"
#include "config/hardware_config.h"

#include <Arduino.h>
#include <math.h>
#include <string.h>

// ============================================================
//  Реалізація DSP ефектів
//  Ланцюг: EQ → Drive → Bitcrush → Delay → Reverb
//          → MasterEQ → Compressor → Saturation → Limiter
// ============================================================

// ── Поточні параметри ─────────────────────────────────────────
static DSPParams s_params;

// ── Стан ревербератора (3 паралельних linesm Schroeder-style) ─
#define REVERB_LINE1 2237 // ~50 мс при 44100 Гц
#define REVERB_LINE2 3271 // ~74 мс
#define REVERB_LINE3 4507 // ~102 мс
static float s_revBuf1[REVERB_LINE1];
static float s_revBuf2[REVERB_LINE2];
static float s_revBuf3[REVERB_LINE3];
static uint32_t s_revIdx1 = 0, s_revIdx2 = 0, s_revIdx3 = 0;

// ── Стан delay ────────────────────────────────────────────────
// 250 мс при 44100: 11025 стерео-фреймів
#define DELAY_FRAMES 11025
static float s_delBufL[DELAY_FRAMES];
static float s_delBufR[DELAY_FRAMES];
static uint32_t s_delIdx = 0;

// ── Стан biquad-фільтрів EQ (для кожної смуги, стерео) ───────
struct Biquad {
  float a0, a1, a2, b1, b2; // Коефіцієнти
  float z1L = 0, z2L = 0;   // Стан лівого каналу
  float z1R = 0, z2R = 0;   // Стан правого каналу
};

static Biquad s_eqLow, s_eqMid, s_eqHigh;
static Biquad s_masterEqLow, s_masterEqMid, s_masterEqHigh;

// ── Стан компресора ───────────────────────────────────────────
static float s_compEnvelope = 0.0f;
static float s_compGain = 1.0f;

// ── Допоміжні функції ─────────────────────────────────────────

// Розрахунок low-shelf biquad (для низьких частот)
static void calcLowShelf(Biquad &bq, float gainDb, float freqHz) {
  const float Fs = (float)SAMPLE_RATE;
  const float A = powf(10.0f, gainDb / 40.0f);
  const float w0 = 2.0f * M_PI * freqHz / Fs;
  const float cosw = cosf(w0);
  const float sinw = sinf(w0);
  const float S = 1.0f; // Полиця Q
  const float alpha =
      sinw / 2.0f * sqrtf((A + 1.0f / A) * (1.0f / S - 1.0f) + 2.0f);
  const float sqrtA = sqrtf(A);
  const float b0 = A * ((A + 1) - (A - 1) * cosw + 2 * sqrtA * alpha);
  const float b1 = 2.0f * A * ((A - 1) - (A + 1) * cosw);
  const float b2 = A * ((A + 1) - (A - 1) * cosw - 2 * sqrtA * alpha);
  const float a0 = (A + 1) + (A - 1) * cosw + 2 * sqrtA * alpha;
  const float a1 = -2.0f * ((A - 1) + (A + 1) * cosw);
  const float a2 = (A + 1) + (A - 1) * cosw - 2 * sqrtA * alpha;
  bq.a0 = b0 / a0;
  bq.a1 = b1 / a0;
  bq.a2 = b2 / a0;
  bq.b1 = a1 / a0;
  bq.b2 = a2 / a0;
}

// Розрахунок high-shelf biquad
static void calcHighShelf(Biquad &bq, float gainDb, float freqHz) {
  const float Fs = (float)SAMPLE_RATE;
  const float A = powf(10.0f, gainDb / 40.0f);
  const float w0 = 2.0f * M_PI * freqHz / Fs;
  const float cosw = cosf(w0);
  const float sinw = sinf(w0);
  const float alpha =
      sinw / 2.0f * sqrtf((A + 1.0f / A) * (1.0f - 1.0f) + 2.0f);
  const float sqrtA = sqrtf(A);
  const float b0 = A * ((A + 1) + (A - 1) * cosw + 2 * sqrtA * alpha);
  const float b1 = -2.0f * A * ((A - 1) + (A + 1) * cosw);
  const float b2 = A * ((A + 1) + (A - 1) * cosw - 2 * sqrtA * alpha);
  const float a0 = (A + 1) - (A - 1) * cosw + 2 * sqrtA * alpha;
  const float a1 = 2.0f * ((A - 1) - (A + 1) * cosw);
  const float a2 = (A + 1) - (A - 1) * cosw - 2 * sqrtA * alpha;
  bq.a0 = b0 / a0;
  bq.a1 = b1 / a0;
  bq.a2 = b2 / a0;
  bq.b1 = a1 / a0;
  bq.b2 = a2 / a0;
}

// Розрахунок peaking EQ biquad (для середніх частот)
static void calcPeakEQ(Biquad &bq, float gainDb, float freqHz, float Q) {
  const float Fs = (float)SAMPLE_RATE;
  const float A = powf(10.0f, gainDb / 40.0f);
  const float w0 = 2.0f * M_PI * freqHz / Fs;
  const float alpha = sinf(w0) / (2.0f * Q);
  bq.a0 = 1.0f + alpha * A;
  bq.a1 = -2.0f * cosf(w0);
  bq.a2 = 1.0f - alpha * A;
  const float e0 = 1.0f + alpha / A;
  bq.b1 = bq.a1;
  bq.b2 = 1.0f - alpha / A;
  bq.a0 /= e0;
  bq.a1 /= e0;
  bq.a2 /= e0;
  bq.b1 /= e0;
  bq.b2 /= e0;
}

// Застосувати biquad до двох каналів (inline)
static inline void processBiquad(Biquad &bq, float &sL, float &sR) {
  float outL =
      bq.a0 * sL + bq.a1 * sL + bq.a2 * sL - bq.b1 * bq.z1L - bq.b2 * bq.z2L;
  /* Спрощена Direct Form I: правильний розрахунок */
  outL = bq.a0 * sL - bq.b1 * bq.z1L - bq.b2 * bq.z2L;
  bq.z2L = bq.z1L;
  bq.z1L = bq.a0 * sL + bq.a1 * bq.z1L + bq.a2 * bq.z2L - bq.b1 * outL -
           bq.b2 * bq.z2L;

  // Direct Form II Transposed (стабільніша для вбудованих систем)
  float wL = sL - bq.b1 * bq.z1L - bq.b2 * bq.z2L;
  outL = bq.a0 * wL + bq.a1 * bq.z1L + bq.a2 * bq.z2L;
  bq.z2L = bq.z1L;
  bq.z1L = wL;
  sL = outL;

  float wR = sR - bq.b1 * bq.z1R - bq.b2 * bq.z2R;
  float outR = bq.a0 * wR + bq.a1 * bq.z1R + bq.a2 * bq.z2R;
  bq.z2R = bq.z1R;
  bq.z1R = wR;
  sR = outR;
}

// Перерахувати всі biquad-фільтри з поточних параметрів
static void updateFilters() {
  // EQ: низькі 200 Гц, середні 1кГц, високі 8кГц
  calcLowShelf(s_eqLow, s_params.eqLow * 12.0f, 200.0f);
  calcPeakEQ(s_eqMid, s_params.eqMid * 12.0f, 1000.0f, 0.7f);
  calcHighShelf(s_eqHigh, s_params.eqHigh * 12.0f, 8000.0f);
  // Мастер EQ: 100 Гц, 3кГц, 12кГц
  calcLowShelf(s_masterEqLow, s_params.masterEqLow * 12.0f, 100.0f);
  calcPeakEQ(s_masterEqMid, s_params.masterEqMid * 12.0f, 3000.0f, 0.7f);
  calcHighShelf(s_masterEqHigh, s_params.masterEqHigh * 12.0f, 12000.0f);
}

// ── Публічний API ─────────────────────────────────────────────
void dspInit() {
  memset(s_revBuf1, 0, sizeof(s_revBuf1));
  memset(s_revBuf2, 0, sizeof(s_revBuf2));
  memset(s_revBuf3, 0, sizeof(s_revBuf3));
  memset(s_delBufL, 0, sizeof(s_delBufL));
  memset(s_delBufR, 0, sizeof(s_delBufR));
  s_revIdx1 = s_revIdx2 = s_revIdx3 = 0;
  s_delIdx = 0;
  s_compEnvelope = 0.0f;
  s_compGain = 1.0f;
  s_params = DSPParams{};
  updateFilters();
}

void dspSetParams(const DSPParams &params) {
  s_params = params;
  updateFilters();
}

const DSPParams &dspGetParams() { return s_params; }

void dspSetDrive(float v) { s_params.drive = v; }
void dspSetReverb(float v) { s_params.reverb = v; }
void dspSetDelay(float v, float fb) {
  s_params.delay = v;
  s_params.delayFb = fb;
}
void dspSetBitcrush(float v) { s_params.bitcrush = v; }
void dspSetMasterVol(float v) { s_params.masterVol = v; }
void dspSetSaturation(float v) { s_params.saturation = v; }
void dspSetLimiter(float threshold) { s_params.limiterThresh = threshold; }

void dspSetEQ(float low, float mid, float high) {
  s_params.eqLow = low;
  s_params.eqMid = mid;
  s_params.eqHigh = high;
  calcLowShelf(s_eqLow, low * 12.0f, 200.0f);
  calcPeakEQ(s_eqMid, mid * 12.0f, 1000.0f, 0.7f);
  calcHighShelf(s_eqHigh, high * 12.0f, 8000.0f);
}

void dspSetMasterEQ(float low, float mid, float high) {
  s_params.masterEqLow = low;
  s_params.masterEqMid = mid;
  s_params.masterEqHigh = high;
  calcLowShelf(s_masterEqLow, low * 12.0f, 100.0f);
  calcPeakEQ(s_masterEqMid, mid * 12.0f, 3000.0f, 0.7f);
  calcHighShelf(s_masterEqHigh, high * 12.0f, 12000.0f);
}

void dspSetCompressor(float threshold, float ratio, float attackMs,
                      float releaseMs) {
  s_params.compThreshold = threshold;
  s_params.compRatio = ratio;
  s_params.compAttackMs = attackMs;
  s_params.compReleaseMs = releaseMs;
}

// ── Головна обробка ───────────────────────────────────────────
void dspProcess(int16_t *buffer, size_t frames) {
  // Коефіцієнти огинаючої компресора
  const float attCoeff =
      expf(-1.0f / (s_params.compAttackMs * 0.001f * SAMPLE_RATE));
  const float relCoeff =
      expf(-1.0f / (s_params.compReleaseMs * 0.001f * SAMPLE_RATE));
  const float thresh = s_params.compThreshold;
  const float ratio = s_params.compRatio;

  for (size_t i = 0; i < frames; i++) {
    float sL = buffer[i * 2 + 0] / 32768.0f;
    float sR = buffer[i * 2 + 1] / 32768.0f;

    // ── 1. EQ (канальний) ─────────────────────────────────
    if (s_params.eqLow != 0.0f)
      processBiquad(s_eqLow, sL, sR);
    if (s_params.eqMid != 0.0f)
      processBiquad(s_eqMid, sL, sR);
    if (s_params.eqHigh != 0.0f)
      processBiquad(s_eqHigh, sL, sR);

    // ── 2. Drive (overdrive/saturation) ───────────────────
    if (s_params.drive > 0.001f) {
      const float drv = 1.0f + s_params.drive * 14.0f;
      const float mix = s_params.drive * 0.85f;
      float dL = tanhf(sL * drv) * (1.0f / tanhf(drv));
      float dR = tanhf(sR * drv) * (1.0f / tanhf(drv));
      sL = sL * (1.0f - mix) + dL * mix;
      sR = sR * (1.0f - mix) + dR * mix;
    }

    // ── 3. Bitcrush ───────────────────────────────────────
    if (s_params.bitcrush > 0.001f) {
      int bits = (int)(16.0f - s_params.bitcrush * 12.0f);
      if (bits < 4)
        bits = 4;
      const float step = 1.0f / (float)(1 << (bits - 1));
      sL = roundf(sL / step) * step;
      sR = roundf(sR / step) * step;
    }

    // ── 4. Delay (стерео, ping-pong) ─────────────────────
    if (s_params.delay > 0.001f) {
      float dL = s_delBufL[s_delIdx];
      float dR = s_delBufR[s_delIdx];
      // Ping-pong: затримка лівого йде в правий і навпаки
      s_delBufL[s_delIdx] = sR + dL * s_params.delayFb;
      s_delBufR[s_delIdx] = sL + dR * s_params.delayFb;
      s_delIdx = (s_delIdx + 1) % DELAY_FRAMES;
      sL += dL * s_params.delay;
      sR += dR * s_params.delay;
    }

    // ── 5. Reverb (паралельні лінії затримки) ────────────
    if (s_params.reverb > 0.001f) {
      float mono = (sL + sR) * 0.5f;
      float r1 = s_revBuf1[s_revIdx1];
      float r2 = s_revBuf2[s_revIdx2];
      float r3 = s_revBuf3[s_revIdx3];
      const float fb = 0.75f + s_params.reverb * 0.2f;
      s_revBuf1[s_revIdx1] = mono + r1 * fb;
      s_revBuf2[s_revIdx2] = mono + r2 * fb;
      s_revBuf3[s_revIdx3] = mono + r3 * fb;
      s_revIdx1 = (s_revIdx1 + 1) % REVERB_LINE1;
      s_revIdx2 = (s_revIdx2 + 1) % REVERB_LINE2;
      s_revIdx3 = (s_revIdx3 + 1) % REVERB_LINE3;
      float wet = (r1 + r2 * 0.7f + r3 * 0.5f) * s_params.reverb * 0.25f;
      sL += wet;
      sR += wet;
    }

    // ── 6. Мастер EQ ─────────────────────────────────────
    if (s_params.masterEqLow != 0.0f)
      processBiquad(s_masterEqLow, sL, sR);
    if (s_params.masterEqMid != 0.0f)
      processBiquad(s_masterEqMid, sL, sR);
    if (s_params.masterEqHigh != 0.0f)
      processBiquad(s_masterEqHigh, sL, sR);

    // ── 7. Компресор (RMS side-chain) ─────────────────────
    if (thresh < 0.99f) {
      float rms = sqrtf((sL * sL + sR * sR) * 0.5f);
      if (rms > s_compEnvelope) {
        s_compEnvelope = attCoeff * s_compEnvelope + (1.0f - attCoeff) * rms;
      } else {
        s_compEnvelope = relCoeff * s_compEnvelope + (1.0f - relCoeff) * rms;
      }
      float gainDb = 0.0f;
      if (s_compEnvelope > thresh) {
        gainDb = (thresh - s_compEnvelope) * (1.0f - 1.0f / ratio);
      }
      s_compGain = powf(10.0f, gainDb / 20.0f);
      sL *= s_compGain;
      sR *= s_compGain;
    }

    // ── 8. Аналогове насичення ────────────────────────────
    if (s_params.saturation > 0.001f) {
      const float k = s_params.saturation * 3.0f;
      sL = tanhf(sL * (1.0f + k)) / (1.0f + k * 0.3f);
      sR = tanhf(sR * (1.0f + k)) / (1.0f + k * 0.3f);
    }

    // ── 9. Мастер-гучність ────────────────────────────────
    sL *= s_params.masterVol;
    sR *= s_params.masterVol;

    // ── 10. True Peak Limiter ─────────────────────────────
    const float lim = s_params.limiterThresh;
    if (sL > lim)
      sL = lim + (sL - lim) * 0.1f;
    if (sL < -lim)
      sL = -lim + (sL + lim) * 0.1f;
    if (sR > lim)
      sR = lim + (sR - lim) * 0.1f;
    if (sR < -lim)
      sR = -lim + (sR + lim) * 0.1f;
    // Жорстке відсічення
    if (sL > 1.0f)
      sL = 1.0f;
    if (sL < -1.0f)
      sL = -1.0f;
    if (sR > 1.0f)
      sR = 1.0f;
    if (sR < -1.0f)
      sR = -1.0f;

    // ── Конвертація назад у int16 ─────────────────────────
    buffer[i * 2 + 0] = (int16_t)(sL * 32767.0f);
    buffer[i * 2 + 1] = (int16_t)(sR * 32767.0f);
  }
}
