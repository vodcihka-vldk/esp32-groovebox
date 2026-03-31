#pragma once

// ============================================================
//  Апаратна конфігурація ESP32 Groovebox
//  ВАЖЛИВО: Це єдине місце для визначення пінів і констант.
//  Не дублюйте #define в інших файлах!
// ============================================================

// ── LCD 1602 (I2C) ───────────────────────────────────────────
#define LCD_I2C_ADDR    0x27    // Поширена адреса 1602 LCD
#define LCD_COLS        16
#define LCD_ROWS        2
#define I2C_SDA_PIN     21      // Шина Wire: SDA
#define I2C_SCL_PIN     22      // Шина Wire: SCL

// ── I2S ЦАП (зовнішній PCM5102 або MAX98357A) ──────────────
#define I2S_BCLK_PIN    26      // Bit clock
#define I2S_WS_PIN      25      // Word select (LR clock)
#define I2S_DOUT_PIN    27      // Data out
// MCLK не використовується (PCM5102 має внутрішній PLL)

// ── SD-карта (SPI через VSPI) ────────────────────────────────
// ВАЖЛИВО: ці піни НЕ повинні перетинатися з матрицею кнопок!
#define SD_CS_PIN       15      // Chip select (не GPIO5, щоб не конфліктувати)
#define SD_MOSI_PIN     13      // VSPI MOSI
#define SD_MISO_PIN     12      // VSPI MISO
#define SD_SCK_PIN      14      // VSPI CLK

// ── Ротарний енкодер ─────────────────────────────────────────
#define ENCODER_CLK_PIN  32     // A-фаза (CLK)
#define ENCODER_DT_PIN   33     // B-фаза (DT)
#define ENCODER_SW_PIN   34     // Кнопка (INPUT тільки — немає PULLUP на GPIO34)

// ── Матриця кнопок 4×4 (16 пад-кнопок) ──────────────────────
// Рядки (Row) — вихід, стовпці (Col) — вхід з PULLUP
// УВАГА: піни обрані без конфлікту з SPI або I2S!
#define MATRIX_ROWS     4
#define MATRIX_COLS     4

// Рядки матриці (виходи)
static const uint8_t k_rowPins[MATRIX_ROWS] = { 16, 17, 4,  2 };
// Стовпці матриці (входи)
static const uint8_t k_colPins[MATRIX_COLS] = { 35, 39, 36, 0 };

// ── Аудіо параметри ───────────────────────────────────────────
#define SAMPLE_RATE         44100   // Гц
#define DMA_BUF_COUNT       8       // Кількість DMA-буферів
#define DMA_BUF_LEN         256     // Зразків у DMA-буфері (менше = менша затримка)
#define AUDIO_BUF_SIZE      512     // Зразків у внутрішньому буфері обробки

// ── Розміри проекту ───────────────────────────────────────────
#define MAX_PATTERNS        8       // Паттернів у проекті
#define MAX_CHANNELS        8       // Кількість каналів (доріжок)
#define MAX_STEPS           64      // Кроків у паттерні
#define MAX_PROJECTS        16      // Проектів на SD-карті
#define PROJECT_NAME_LEN    16      // Символів у назві проекту
#define MAX_POLYPHONY       16      // Максимальна поліфонія

// ── FreeRTOS задачі ───────────────────────────────────────────
// Ядро 0 → аудіо (найвищий пріоритет, без переривань UI)
// Ядро 1 → UI + секвенсор (відокремлені пріоритетами)
#define AUDIO_TASK_CORE     0
#define AUDIO_TASK_PRIORITY 5
#define AUDIO_TASK_STACK    8192    // Виправлено: 8KB для float DSP

#define SEQ_TASK_CORE       1
#define SEQ_TASK_PRIORITY   3
#define SEQ_TASK_STACK      4096

#define UI_TASK_CORE        1
#define UI_TASK_PRIORITY    2       // Нижчий за секвенсор
#define UI_TASK_STACK       4096

// ── Обмеження BPM ─────────────────────────────────────────────
#define BPM_MIN     40
#define BPM_MAX     240
#define BPM_DEFAULT 120

// ── Шляхи SD-карти ────────────────────────────────────────────
#define SD_ROOT_DIR         "/GROOVEBOX"
#define SD_PROJECT_EXT      ".gbx"

// ── Захист від помилок пінів ──────────────────────────────────
// Статична перевірка що SD і матриця не перетинаються
static_assert(SD_CS_PIN   != k_rowPins[0] &&
              SD_CS_PIN   != k_rowPins[1] &&
              SD_CS_PIN   != k_rowPins[2] &&
              SD_CS_PIN   != k_rowPins[3],
              "КОНФЛІКТ: SD_CS_PIN перетинається з рядками матриці!");
