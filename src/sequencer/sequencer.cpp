#include "sequencer.h"
#include "audio/sample_engine.h"
#include "config/hardware_config.h"

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// ============================================================
//  Реалізація секвенсора
// ============================================================

static GrooveboxState *s_state = nullptr;

// ── Розрахунок інтервалу кроку в мікросекундах ───────────────
// 16-ті ноти при BPM:  60_000_000 мкс / (BPM * 4)
// Ціла арифметика втратить точність → повертаємо uint32_t мкс
static inline uint32_t stepIntervalUs(uint16_t bpm) {
  // 60 000 000 / (bpm * 4) — обчислюємо через 64-бітні зсуви
  return (uint32_t)(60000000UL / ((uint32_t)bpm * 4u));
}

// ── Зсув свінгу для чётних кроків ────────────────────────────
// swing 0..100 → зсув 0..33% від stepInterval
static inline uint32_t swingOffsetUs(uint16_t bpm, uint8_t swing,
                                     uint8_t step) {
  if (swing == 0)
    return 0;
  // Тільки для парних кроків (0, 2, 4...) додаємо зсув
  if ((step & 1) == 0)
    return 0;
  uint32_t interval = stepIntervalUs(bpm);
  return (interval * swing) / 300u; // Максимум ~33%
}

// ── Ініціалізація ─────────────────────────────────────────────
void seqInit(GrooveboxState *state) { s_state = state; }

// ── FreeRTOS задача секвенсора ────────────────────────────────
void seqTask(void *param) {
  (void)param;
  Serial.println("[Sequencer] Задачу запущено");

  // Поточний час у мікросекундах — точніший за millis()
  uint64_t nextStepUs = (uint64_t)esp_timer_get_time();

  while (true) {
    if (!s_state || !s_state->seq.isPlaying) {
      // Чекаємо 5 мс, не навантажуємо ядро
      vTaskDelay(pdMS_TO_TICKS(5));
      nextStepUs = (uint64_t)esp_timer_get_time();
      continue;
    }

    const uint64_t nowUs = (uint64_t)esp_timer_get_time();

    if (nowUs >= nextStepUs) {
      SequencerState &seq = s_state->seq;
      const Pattern &pat = s_state->patterns[seq.currentPat];

      // ── Тригер зразків поточного кроку ───────────────
      for (uint8_t ch = 0; ch < MAX_CHANNELS; ch++) {
        const Step &step = pat.steps[ch][seq.currentStep];
        if (!step.active)
          continue;

        const ChannelState &chan = s_state->channels[ch];

        // Пропускаємо заглушений канал
        if (chan.mute)
          continue;

        // Перевірка соло: якщо є будь-який соло — грають тільки вони
        bool anySolo = false;
        for (uint8_t c = 0; c < MAX_CHANNELS; c++) {
          if (s_state->channels[c].solo) {
            anySolo = true;
            break;
          }
        }
        if (anySolo && !chan.solo)
          continue;

        // ── Параметри з урахуванням param-lock ────────
        float vol = chan.volume / 100.0f;
        float pitch = (float)(chan.pitch) - 50.0f; // Тони: -50..+50
        float pan = (chan.pan / 50.0f) - 1.0f;     // -1.0..+1.0

        // Velocity з кроку (0-127 → 0.0-1.0)
        float vel = (float)step.velocity / 127.0f;
        vol *= vel;

        // Параметричні локи — перевизначають параметри каналу
        if (step.hasParamLock) {
          if (step.lockVolume != 0xFF)
            vol = step.lockVolume / 100.0f;
          if (step.lockPitch != 0xFF)
            pitch = (float)step.lockPitch - 50.0f;
        }

        // Атака / спад з параметрів каналу
        // attack 0-100 → 0-200 мс, decay 0-100 → 0-2000 мс
        uint32_t attackMs = (uint32_t)chan.attack * 2u;
        uint32_t decayMs = (uint32_t)chan.decay * 20u;

        sampleEngineTrigger(ch, chan.sampleIndex, vol, pitch, attackMs, decayMs,
                            pan);
      }

      // ── Перехід на наступний крок ─────────────────────
      // ВИПРАВЛЕННЯ: крок інкрементується ПІСЛЯ тригеру,
      // тому крок 0 гарантовано програється при старті
      seq.currentStep = (seq.currentStep + 1) % pat.length;

      // ── Розрахунок часу наступного кроку з урахуванням свінгу
      uint32_t interval = stepIntervalUs(seq.bpm);
      uint32_t swing = swingOffsetUs(seq.bpm, seq.swing, seq.currentStep);
      nextStepUs += interval + swing;
    }

    // Мікросон щоб не спалювати 100% CPU
    // Перевіряємо кожну 1 мс (точність достатня для 240 BPM)
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

// ── Управління ────────────────────────────────────────────────
void seqPlay() {
  if (!s_state)
    return;
  s_state->seq.isPlaying = true;
  s_state->seq.currentStep = 0; // Завжди старт з кроку 0
  Serial.println("[Sequencer] PLAY");
}

void seqStop() {
  if (!s_state)
    return;
  s_state->seq.isPlaying = false;
  s_state->seq.currentStep = 0;
  sampleEngineStopAll();
  Serial.println("[Sequencer] STOP");
}

void seqToggle() {
  if (!s_state)
    return;
  if (s_state->seq.isPlaying)
    seqStop();
  else
    seqPlay();
}

bool seqIsPlaying() { return s_state && s_state->seq.isPlaying; }

void seqJumpToStep(uint8_t step) {
  if (s_state && step < MAX_STEPS) {
    s_state->seq.currentStep = step;
  }
}

uint8_t seqCurrentStep() { return s_state ? s_state->seq.currentStep : 0; }
