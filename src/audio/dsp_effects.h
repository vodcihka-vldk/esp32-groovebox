#pragma once

#include <stddef.h>
#include <stdint.h>

// ============================================================
//  DSP ефекти Groovebox
//  Єдиний модуль для всіх ефектів обробки звуку:
//   Drive → Bitcrush → Reverb → Delay → EQ → Limiter
// ============================================================

// ── Параметри DSP (float, нормалізовані 0.0..1.0) ─────────────
struct DSPParams {
  float drive = 0.0f; // 0.0 = чисто, 1.0 = максимальний overdrive
  float reverb = 0.0f;   // 0.0 = dry, 1.0 = повний reverb
  float delay = 0.0f;    // 0.0 = без delay, 1.0 = max mix
  float delayFb = 0.3f;  // Зворотній зв'язок delay (0.0..0.95)
  float bitcrush = 0.0f; // 0.0 = 16-бітний, 1.0 = 4-бітний
  float eqLow = 0.0f;    // -12dB..+12dB (0 = flat)
  float eqMid = 0.0f;
  float eqHigh = 0.0f;
  float masterVol = 0.8f; // Мастер-гучність

  // Мастер-ланцюг
  float masterEqLow = 0.0f;
  float masterEqMid = 0.0f;
  float masterEqHigh = 0.0f;
  float compThreshold = 0.7f; // RMS-поріг (0.0..1.0)
  float compRatio = 4.0f;     // Ступінь стиснення (1:N)
  float compAttackMs = 5.0f;  // Мс
  float compReleaseMs = 100.0f;
  float saturation = 0.0f; // Аналогове насичення (0.0..1.0)
  float limiterThresh = 0.95f; // Поріг лімітера (0.0..1.0)
};

// ── Ініціалізація (очистити буфери ефектів) ───────────────────
void dspInit();

// ── Оновити параметри (викликати при зміні через UI) ──────────
void dspSetParams(const DSPParams &params);

// ── Отримати поточні параметри ────────────────────────────────
const DSPParams &dspGetParams();

// ── Обробити стерео-буфер (interleaved L,R) ───────────────────
// Застосовує весь ланцюг: EQ → Drive → Bitcrush → Reverb → Delay
//                         → MasterEQ → Compressor → Saturation → Limiter
void dspProcess(int16_t *buffer, size_t frames);

// ── Зручні сетери для UI ───────────────────────────────────────
void dspSetDrive(float v);
void dspSetReverb(float v);
void dspSetDelay(float v, float fb);
void dspSetBitcrush(float v);
void dspSetEQ(float low, float mid, float high);
void dspSetMasterVol(float v);
void dspSetMasterEQ(float low, float mid, float high);
void dspSetCompressor(float threshold, float ratio, float attackMs,
                      float releaseMs);
void dspSetSaturation(float v);
void dspSetLimiter(float threshold);
