#pragma once

#include "config/hardware_config.h"
#include <stddef.h>
#include <stdint.h>

// ============================================================
//  Рушій відтворення семплів (Sample Engine)
//  Відповідає за:
//   - ведення банку вбудованих семплів
//   - поліфонне відтворення до MAX_POLYPHONY голосів
//   - мікшування каналів з огинаючою (attack/decay)
//   - передачу буфера до DSP та I2S DMA
// ============================================================

// ── Метадані одного вбудованого семпла ───────────────────────
struct SampleInfo {
  const int16_t *data; // PCM-дані у PROGMEM/Flash
  uint32_t length;     // Кількість 16-бітних зразків
  const char *name;    // Назва для відображення
  uint8_t baseNote;    // MIDI-нота оригіналу (60 = C4)
};

// ── Стан одного голосу (voice) ────────────────────────────────
struct Voice {
  const SampleInfo *sample = nullptr;
  uint32_t position = 0; // Поточна позиція (цілочисел.)
  uint32_t positionFrac = 0; // Дробова частина (16.16 фікс.)
  uint32_t startSample = 0;  // Початок (від length%)
  uint32_t endSample = 0;    // Кінець (від length%)
  uint32_t pitchIncrement = 0x10000; // Крок у 16.16 (0x10000 = 1x)
  float volume = 1.0f;
  float pan = 0.0f;        // -1.0 (лів) .. +1.0 (прав)
  float envelope = 0.0f;   // Поточне значення огинаючої
  float attackRate = 0.0f; // Приріст за зразок (attack)
  float decayRate = 0.0f;  // Спад за зразок (decay)
  uint8_t channel = 0;     // Канал-власник
  bool active = false;
  bool inDecay = false;

  // Скинути голос у невикористаний стан
  void reset() {
    active = false;
    sample = nullptr;
    position = 0;
    positionFrac = 0;
    inDecay = false;
    envelope = 0.0f;
  }
};

// ── Банк семплів ──────────────────────────────────────────────
// Кількість вбудованих інструментів
#define NUM_BUILTIN_SAMPLES 8

extern const SampleInfo g_builtinSamples[NUM_BUILTIN_SAMPLES];

// ── API рушія ─────────────────────────────────────────────────

// Ініціалізація: очищення всіх голосів, DMA-буферів, I2S
bool sampleEngineInit();

// Запустити відтворення семпла на каналі
// channel    — 0..MAX_CHANNELS-1
// sampleIdx  — 0..NUM_BUILTIN_SAMPLES-1
// volume     — 0.0 .. 1.0
// pitchSemis — зсув тону в тонах (±24)
// attack     — мс атаки (0 = миттєвий)
// decay      — мс спаду  (0 = без спаду)
// panVal     — -1.0 .. +1.0
void sampleEngineTrigger(uint8_t channel, uint8_t sampleIdx, float volume,
                         float pitchSemis, uint32_t attackMs, uint32_t decayMs,
                         float panVal);

// Заповнити стерео-буфер наступними bufferSize зразками (stereo interleaved)
// audioOut — масив розміром bufferSize * 2 (L,R,L,R,...)
void sampleEngineProcess(int16_t *audioOut, size_t bufferSize);

// Зупинити всі голоси
void sampleEngineStopAll();

// Зупинити всі голоси одного каналу
void sampleEngineStopChannel(uint8_t channel);

// Кількість активних голосів (для діагностики)
uint8_t sampleEngineActiveVoices();
