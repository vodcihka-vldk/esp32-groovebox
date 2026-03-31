#include "sample_engine.h"
#include "config/hardware_config.h"
#include "dsp_effects.h"
#include "sample_data/samples_data.h"

#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>
#include <string.h>

// ============================================================
//  Реалізація рушія семплів
// ============================================================

// ── Банк вбудованих семплів ───────────────────────────────────
const SampleInfo g_builtinSamples[NUM_BUILTIN_SAMPLES] = {
    {kick_data, kick_data_length, "KICK", 36},
    {snare_data, snare_data_length, "SNARE", 38},
    {hihat_closed_data, hihat_closed_data_length, "HIHAT-C", 42},
    {hihat_open_data, hihat_open_data_length, "HIHAT-O", 46},
    {tom_data, tom_data_length, "TOM", 45},
    {clap_data, clap_data_length, "CLAP", 39},
    {crash_data, crash_data_length, "CRASH", 49},
    {nullptr, 0, "EMPTY", 60},
};

// ── Масив активних голосів ────────────────────────────────────
static Voice s_voices[MAX_POLYPHONY];

// ── I2S конфігурація ──────────────────────────────────────────
static const i2s_config_t k_i2sConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT, // Стерео
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN,
    .use_apll = false,
    .tx_desc_auto_clear = true, // Заповнює нулями при underrun
    .fixed_mclk = 0};

static const i2s_pin_config_t k_i2sPins = {.bck_io_num = I2S_BCLK_PIN,
                                           .ws_io_num = I2S_WS_PIN,
                                           .data_out_num = I2S_DOUT_PIN,
                                           .data_in_num = I2S_PIN_NO_CHANGE};

// ── Допоміжна функція: конвертація тонів → pitch increment ────
// Повертає фіксований 16.16 множник швидкості відтворення
// Наприклад: 12 тонів → 2x, 0 → 1x, -12 → 0.5x
static uint32_t semisToIncrement(float semitones) {
  // powf(2, semis/12) через таблицю або апроксимацію
  // Точна формула: rate = 2^(semis/12)
  float rate = powf(2.0f, semitones / 12.0f);
  // Конвертуємо у Q16.16 фіксовану точку
  return (uint32_t)(rate * 65536.0f);
}

// ── Ініціалізація ──────────────────────────────────────────────
bool sampleEngineInit() {
  // Скинути всі голоси
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    s_voices[i].reset();
  }

  // Ініціалізація I2S
  esp_err_t err = i2s_driver_install(I2S_NUM_0, &k_i2sConfig, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("[AudioEngine] Помилка i2s_driver_install: %d\n", err);
    return false;
  }
  err = i2s_set_pin(I2S_NUM_0, &k_i2sPins);
  if (err != ESP_OK) {
    Serial.printf("[AudioEngine] Помилка i2s_set_pin: %d\n", err);
    return false;
  }

  // Ініціалізація DSP
  dspInit();

  Serial.println("[AudioEngine] Ініціалізовано успішно");
  return true;
}

// ── Запуск голосу ─────────────────────────────────────────────
void sampleEngineTrigger(uint8_t channel, uint8_t sampleIdx, float volume,
                         float pitchSemis, uint32_t attackMs, uint32_t decayMs,
                         float panVal) {
  // Перевірка входів
  if (channel >= MAX_CHANNELS)
    return;
  if (sampleIdx >= NUM_BUILTIN_SAMPLES)
    return;
  const SampleInfo &info = g_builtinSamples[sampleIdx];
  if (!info.data || info.length == 0)
    return;

  // 1. Шукаємо вільний голос
  int slot = -1;
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    if (!s_voices[i].active) {
      slot = i;
      break;
    }
  }

  // 2. Якщо немає вільного — витісняємо найстаріший голос цього ж каналу
  if (slot == -1) {
    uint32_t maxPos = 0;
    for (int i = 0; i < MAX_POLYPHONY; i++) {
      // Найстаріший = той, чий position найбільший відносно endSample
      uint32_t relPos = s_voices[i].position;
      if (s_voices[i].channel == channel && relPos > maxPos) {
        maxPos = relPos;
        slot = i;
      }
    }
  }

  // 3. Якщо досі немає — витісняємо абсолютно найстаріший (будь-який канал)
  if (slot == -1) {
    uint32_t maxPos = 0;
    for (int i = 0; i < MAX_POLYPHONY; i++) {
      if (s_voices[i].position > maxPos) {
        maxPos = s_voices[i].position;
        slot = i;
      }
    }
  }

  if (slot == -1)
    return; // Не має відбутися

  Voice &v = s_voices[slot];
  v.sample = &info;
  v.channel = channel;
  v.volume = volume;
  v.pan = panVal;
  v.pitchIncrement = semisToIncrement(pitchSemis);
  v.position = 0;
  v.positionFrac = 0;
  v.startSample = 0;
  v.endSample = info.length;
  v.active = true;
  v.inDecay = (attackMs == 0);

  // Огинаюча
  if (attackMs > 0) {
    uint32_t attackSamples = (SAMPLE_RATE * attackMs) / 1000u;
    v.attackRate = (attackSamples > 0) ? (1.0f / (float)attackSamples) : 1.0f;
    v.envelope = 0.0f;
  } else {
    v.attackRate = 0.0f;
    v.envelope = 1.0f;
  }
  if (decayMs > 0) {
    uint32_t decaySamples = (SAMPLE_RATE * decayMs) / 1000u;
    v.decayRate = (decaySamples > 0) ? (1.0f / (float)decaySamples) : 1.0f;
  } else {
    v.decayRate = 0.0f; // 0 = без decay, грає до кінця
  }
}

// ── Обробка аудіо-буфера (стерео, interleaved: L,R,L,R,...) ──
void sampleEngineProcess(int16_t *audioOut, size_t bufferSize) {
  // Очищення вихідного буфера
  memset(audioOut, 0, bufferSize * 2 * sizeof(int16_t));

  for (int vi = 0; vi < MAX_POLYPHONY; vi++) {
    Voice &v = s_voices[vi];
    if (!v.active || !v.sample || !v.sample->data)
      continue;

    const int16_t *data = v.sample->data;
    const uint32_t endPos = v.endSample;

    const float panL = (v.pan <= 0.0f) ? 1.0f : (1.0f - v.pan);
    const float panR = (v.pan >= 0.0f) ? 1.0f : (1.0f + v.pan);

    for (size_t j = 0; j < bufferSize; j++) {
      // Закінчення семпла
      if (v.position >= endPos) {
        v.reset();
        break;
      }

      // ── Огинаюча ──────────────────────────────────────
      if (!v.inDecay) {
        v.envelope += v.attackRate;
        if (v.envelope >= 1.0f) {
          v.envelope = 1.0f;
          v.inDecay = true;
        }
      } else if (v.decayRate > 0.0f) {
        v.envelope -= v.decayRate;
        if (v.envelope <= 0.0f) {
          v.reset();
          break;
        }
      }

      // ── Лінійна інтерполяція (pitch) ──────────────────
      uint32_t pos1 = v.position;
      uint32_t pos2 = pos1 + 1;

      // Дроб у Q16
      float frac = (float)(v.positionFrac) / 65536.0f;

      int32_t s1 = data[pos1];
      int32_t s2 = (pos2 < endPos) ? data[pos2] : s1;
      float sample = s1 + (s2 - s1) * frac;

      // Застосовуємо огинаючу та гучність
      sample *= v.volume * v.envelope;

      // ── Стерео-мікшування (interleaved L,R) ───────────
      int32_t outL = audioOut[j * 2 + 0] + (int32_t)(sample * panL);
      int32_t outR = audioOut[j * 2 + 1] + (int32_t)(sample * panR);

      // Soft clamp
      audioOut[j * 2 + 0] =
          (int16_t)(outL > 32767 ? 32767 : (outL < -32768 ? -32768 : outL));
      audioOut[j * 2 + 1] =
          (int16_t)(outR > 32767 ? 32767 : (outR < -32768 ? -32768 : outR));

      // ── Оновлення позиції у Q16.16 ────────────────────
      v.positionFrac += v.pitchIncrement & 0xFFFF;
      v.position += (v.pitchIncrement >> 16) + (v.positionFrac >> 16);
      v.positionFrac &= 0xFFFF;
    }
  }

  // ── Застосувати глобальні DSP ефекти ─────────────────────
  dspProcess(audioOut, bufferSize);

  // ── Вивести у I2S ─────────────────────────────────────────
  size_t bytesWritten = 0;
  i2s_write(I2S_NUM_0, audioOut, bufferSize * 2 * sizeof(int16_t),
            &bytesWritten, portMAX_DELAY);
}

// ── Зупинка ───────────────────────────────────────────────────
void sampleEngineStopAll() {
  for (int i = 0; i < MAX_POLYPHONY; i++)
    s_voices[i].reset();
}

void sampleEngineStopChannel(uint8_t channel) {
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    if (s_voices[i].channel == channel)
      s_voices[i].reset();
  }
}

uint8_t sampleEngineActiveVoices() {
  uint8_t count = 0;
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    if (s_voices[i].active)
      count++;
  }
  return count;
}
