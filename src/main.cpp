// ============================================================
//  ESP32 Groovebox — Точка входу (main.cpp)
//  Тут тільки:
//   1. Визначення глобального стану
//   2. Ініціалізація обладнання (setup)
//   3. Запуск FreeRTOS задач
//   4. Порожній loop (вся логіка у задачах)
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "audio/dsp_effects.h"
#include "audio/sample_engine.h"
#include "config/hardware_config.h"
#include "sequencer/sequencer.h"
#include "state/groovebox_state.h"
#include "storage/project_storage.h"
#include "ui/controls.h"
#include "ui/display.h"

// ── Глобальний стан (визначення) ─────────────────────────────
// Оголошений як extern у groovebox_state.h
GrooveboxState g_state;

// ── Хендли задач ─────────────────────────────────────────────
static TaskHandle_t s_audioTaskHandle = nullptr;
static TaskHandle_t s_seqTaskHandle = nullptr;
static TaskHandle_t s_uiTaskHandle = nullptr;

// ── Аудіо задача (Core 0, найвищий пріоритет) ─────────────────
// Безперервно заповнює DMA-буфер I2S
static void audioTask(void * /*param*/) {
  Serial.println("[AudioTask] Запущено на ядрі 0");

  // Статичний буфер у DRAM (не стек!) — 512 стерео-фреймів = 2KB
  static int16_t audioBuf[AUDIO_BUF_SIZE * 2];

  while (true) {
    // Масштабуємо параметри зі стану (0-100) у float (0.0-1.0 / дБ)
    DSPParams p;
    p.drive = g_state.dsp.driveAmount / 100.0f;
    p.reverb = g_state.dsp.reverbAmount / 100.0f;
    p.delay = g_state.dsp.delayAmount / 100.0f;
    p.delayFb = g_state.dsp.delayFeedback / 100.0f;
    p.bitcrush = g_state.dsp.bitcrushAmount / 100.0f;
    // EQ: 0-100 → -1.0 .. +1.0 (50 = flat = 0.0)
    p.eqLow = (g_state.dsp.eqLow - 50) / 50.0f;
    p.eqMid = (g_state.dsp.eqMid - 50) / 50.0f;
    p.eqHigh = (g_state.dsp.eqHigh - 50) / 50.0f;
    p.masterVol = g_state.masterVolume / 100.0f;
    p.masterEqLow = (g_state.dsp.masterEQLow - 50) / 50.0f;
    p.masterEqMid = (g_state.dsp.masterEQMid - 50) / 50.0f;
    p.masterEqHigh = (g_state.dsp.masterEQHigh - 50) / 50.0f;
    p.compThreshold = g_state.dsp.compThreshold / 100.0f;
    p.compRatio = 1.0f + g_state.dsp.compRatio / 25.0f; // 1:1 .. 1:5
    p.saturation = g_state.dsp.saturation / 100.0f;
    p.limiterThresh = g_state.dsp.limiterThresh / 100.0f;
    dspSetParams(p);

    // Генерувати + мікшувати + вивести в I2S
    sampleEngineProcess(audioBuf, AUDIO_BUF_SIZE);

    // Відсутній vTaskDelay — sampleEngineProcess блокується на i2s_write
    // яка повертається тільки коли DMA-буфер прийнято → природній rate-control
  }
}

// ── Задача секвенсора (Core 1, середній пріоритет) ────────────
static void sequencerTaskWrapper(void *param) {
  seqTask(param); // Делегуємо у sequencer.cpp
}

// ── UI задача (Core 1, найнижчий пріоритет) ───────────────────
static void uiTaskWrapper(void * /*param*/) {
  Serial.println("[UITask] Запущено на ядрі 1");

  uint32_t lastDisplayMs = 0;
  uint32_t lastControlsMs = 0;

  while (true) {
    uint32_t now = millis();

    // Зчитати стан кнопок та енкодера (кожні 10 мс → 100 Гц)
    if (now - lastControlsMs >= 10) {
      controlsPoll(&g_state);
      lastControlsMs = now;
    }

    // Оновити дисплей (кожні 80 мс → 12.5 FPS — достатньо для LCD)
    if (now - lastDisplayMs >= 80) {
      if (g_state.ui.needsRedraw) {
        displayRender(&g_state);
        g_state.ui.needsRedraw = false;
      }
      lastDisplayMs = now;
    }

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// ─────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n========================================");
  Serial.println("  ESP32 Groovebox v2.0  Завантаження...");
  Serial.println("========================================");

  // ── I2C (LCD) ─────────────────────────────────────────────
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // ── Дисплей ───────────────────────────────────────────────
  displayInit();
  displayShowSplash("ESP32 GROOVEBOX", "v2.0  Запуск...");

  // ── Управління (енкодер + матриця) ────────────────────────
  controlsInit();

  // ── Аудіо рушій + I2S ─────────────────────────────────────
  if (!sampleEngineInit()) {
    Serial.println("[Setup] КРИТИЧНО: ініціалізація I2S провалилась!");
    displayShowSplash("ПОМИЛКА", "I2S INIT FAIL");
    while (true)
      vTaskDelay(portMAX_DELAY); // Зупинка
  }

  // ── SD-карта та проекти ────────────────────────────────────
  if (!storageInit(&g_state)) {
    Serial.println("[Setup] ПОПЕРЕДЖЕННЯ: SD-карта недоступна");
    displayShowSplash("ПОПЕРЕДЖЕННЯ", "SD недоступна");
    delay(1500);
  }

  // ── Секвенсор ─────────────────────────────────────────────
  seqInit(&g_state);

  // ── FreeRTOS задачі ────────────────────────────────────────
  xTaskCreatePinnedToCore(
      audioTask,        // Функція
      "AudioTask",      // Назва
      AUDIO_TASK_STACK, // Стек (8KB — достатньо для float DSP)
      nullptr,          // Параметри
      AUDIO_TASK_PRIORITY, // Пріоритет (5 — найвищий)
      &s_audioTaskHandle,
      AUDIO_TASK_CORE // Ядро 0
  );

  xTaskCreatePinnedToCore(sequencerTaskWrapper, "SeqTask", SEQ_TASK_STACK,
                          nullptr,
                          SEQ_TASK_PRIORITY, // Пріоритет 3
                          &s_seqTaskHandle,
                          SEQ_TASK_CORE // Ядро 1
  );

  xTaskCreatePinnedToCore(uiTaskWrapper, "UITask", UI_TASK_STACK, nullptr,
                          UI_TASK_PRIORITY, // Пріоритет 2 (нижчий за seq)
                          &s_uiTaskHandle,
                          UI_TASK_CORE // Ядро 1
  );

  Serial.println("[Setup] Всі задачі запущено. Groovebox готовий!");
  displayShowSplash("GROOVEBOX", "Готовий  :)");
  delay(800);

  // Сигнал готовності — програємо kick
  sampleEngineTrigger(0, 0, 0.8f, 0.0f, 5, 500, 0.0f);
}

void loop() {
  // Весь код у FreeRTOS задачах.
  // loop() не повинен нічого робити — просто сплять.
  vTaskDelay(portMAX_DELAY);
}
