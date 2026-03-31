#include "project_storage.h"
#include "config/hardware_config.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>

// ============================================================
//  Реалізація зберігання проектів
// ============================================================

static GrooveboxState *s_state = nullptr;

// SPI-шина для SD — використовуємо VSPI щоб не конфліктувати з I2S
static SPIClass s_spi(VSPI);

// ── Допоміжна функція: побудова шляху до файлу проекту ───────
static void buildPath(char *buf, size_t bufSz, const char *name) {
  snprintf(buf, bufSz, "%s/%s%s", SD_ROOT_DIR, name, SD_PROJECT_EXT);
}

// ── Ініціалізація ─────────────────────────────────────────────
bool storageInit(GrooveboxState *state) {
  s_state = state;
  if (!s_state)
    return false;

  // Ініціалізація SPI на фізично незайнятих пінах (VSPI)
  s_spi.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);

  if (!SD.begin(SD_CS_PIN, s_spi)) {
    Serial.println("[Storage] Помилка: SD-карта не знайдена");
    s_state->project.sdAvailable = false;
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("[Storage] Помилка: тип карти невизначений");
    s_state->project.sdAvailable = false;
    return false;
  }

  Serial.printf("[Storage] SD-карта: %s, %lluMB\n",
                (cardType == CARD_MMC)    ? "MMC"
                : (cardType == CARD_SD)   ? "SDSC"
                : (cardType == CARD_SDHC) ? "SDHC"
                                          : "Unknown",
                SD.cardSize() / (1024 * 1024));

  // Створити директорію проектів якщо не існує
  if (!SD.exists(SD_ROOT_DIR)) {
    SD.mkdir(SD_ROOT_DIR);
    Serial.printf("[Storage] Створено директорію: %s\n", SD_ROOT_DIR);
  }

  s_state->project.sdAvailable = true;
  storageScanProjects();
  return true;
}

// ── Сканування проектів ───────────────────────────────────────
void storageScanProjects() {
  if (!s_state || !s_state->project.sdAvailable)
    return;

  File root = SD.open(SD_ROOT_DIR);
  if (!root || !root.isDirectory()) {
    Serial.println("[Storage] Помилка: не вдалося відкрити директорію");
    return;
  }

  s_state->project.numProjects = 0;
  const size_t extLen = strlen(SD_PROJECT_EXT);

  File entry = root.openNextFile();
  while (entry && s_state->project.numProjects < MAX_PROJECTS) {
    if (!entry.isDirectory()) {
      String fname = entry.name();
      if (fname.endsWith(SD_PROJECT_EXT)) {
        // Відрізати розширення
        String pname = fname.substring(0, fname.length() - extLen);
        pname.toCharArray(s_state->project.names[s_state->project.numProjects],
                          PROJECT_NAME_LEN);
        s_state->project.numProjects++;
        Serial.printf("[Storage] Знайдено проект: %s\n", pname.c_str());
      }
    }
    // ВИПРАВЛЕННЯ: закриваємо кожен entry щоб не витікали хендли
    entry.close();
    entry = root.openNextFile();
  }
  root.close();

  Serial.printf("[Storage] Всього проектів: %d\n",
                s_state->project.numProjects);
}

// ── JSON серіалізація / десеріалізація ────────────────────────

// Розмір JSON-документа залежить від кількості кроків:
// 8 паттернів × 8 каналів × 64 кроки × ~3 байти + заголовок ≈ 18KB
// Зберігаємо у PSRAM якщо доступно
#define JSON_DOC_SIZE (20 * 1024)

static bool serializeState(File &file, const GrooveboxState &st) {
  // Виділяємо у PSRAM якщо доступно
  DynamicJsonDocument doc(JSON_DOC_SIZE);

  doc["name"] = st.project.names[st.project.currentIndex];
  doc["bpm"] = st.seq.bpm;
  doc["swing"] = st.seq.swing;
  doc["masterVol"] = st.masterVolume;

  // DSP параметри
  JsonObject dsp = doc.createNestedObject("dsp");
  dsp["drive"] = st.dsp.driveAmount;
  dsp["reverb"] = st.dsp.reverbAmount;
  dsp["delay"] = st.dsp.delayAmount;
  dsp["delayFb"] = st.dsp.delayFeedback;
  dsp["bitcrush"] = st.dsp.bitcrushAmount;
  dsp["eqLow"] = st.dsp.eqLow;
  dsp["eqMid"] = st.dsp.eqMid;
  dsp["eqHigh"] = st.dsp.eqHigh;
  dsp["masterEqLow"] = st.dsp.masterEQLow;
  dsp["masterEqMid"] = st.dsp.masterEQMid;
  dsp["masterEqHi"] = st.dsp.masterEQHigh;
  dsp["compThresh"] = st.dsp.compThreshold;
  dsp["compRatio"] = st.dsp.compRatio;
  dsp["sat"] = st.dsp.saturation;
  dsp["limiter"] = st.dsp.limiterThresh;

  // Параметри каналів
  JsonArray channels = doc.createNestedArray("channels");
  for (int ch = 0; ch < MAX_CHANNELS; ch++) {
    JsonObject c = channels.createNestedObject();
    c["sample"] = st.channels[ch].sampleIndex;
    c["vol"] = st.channels[ch].volume;
    c["pitch"] = st.channels[ch].pitch;
    c["drive"] = st.channels[ch].drive;
    c["start"] = st.channels[ch].start;
    c["length"] = st.channels[ch].length;
    c["attack"] = st.channels[ch].attack;
    c["decay"] = st.channels[ch].decay;
    c["pan"] = st.channels[ch].pan;
    c["mute"] = st.channels[ch].mute;
    c["solo"] = st.channels[ch].solo;
  }

  // Паттерни (бітове кодування для стиснення)
  JsonArray patterns = doc.createNestedArray("patterns");
  for (int p = 0; p < MAX_PATTERNS; p++) {
    JsonArray pat = patterns.createNestedArray();
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
      // Кодуємо як uint64 бітову маску (64 кроки → 8 байт)
      uint64_t mask = 0;
      for (int s = 0; s < MAX_STEPS; s++) {
        if (st.patterns[p].steps[ch][s].active) {
          mask |= (1ULL << s);
        }
      }
      pat.add((uint32_t)(mask & 0xFFFFFFFF));
      pat.add((uint32_t)(mask >> 32));
    }
    // Довжина паттерну
    pat.add(st.patterns[p].length);
  }

  size_t written = serializeJson(doc, file);
  if (written == 0) {
    Serial.println("[Storage] Помилка: не вдалося серіалізувати JSON");
    return false;
  }
  return true;
}

static bool deserializeState(File &file, GrooveboxState &st, uint8_t idx) {
  DynamicJsonDocument doc(JSON_DOC_SIZE);
  DeserializationError err = deserializeJson(doc, file);
  if (err) {
    Serial.printf("[Storage] JSON помилка: %s\n", err.c_str());
    return false;
  }

  st.seq.bpm = doc["bpm"] | BPM_DEFAULT;
  st.seq.swing = doc["swing"] | 0;
  st.masterVolume = doc["masterVol"] | 80;

  // DSP
  JsonObjectConst dsp = doc["dsp"];
  st.dsp.driveAmount = dsp["drive"] | 0;
  st.dsp.reverbAmount = dsp["reverb"] | 0;
  st.dsp.delayAmount = dsp["delay"] | 0;
  st.dsp.delayFeedback = dsp["delayFb"] | 30;
  st.dsp.bitcrushAmount = dsp["bitcrush"] | 0;
  st.dsp.eqLow = dsp["eqLow"] | 50;
  st.dsp.eqMid = dsp["eqMid"] | 50;
  st.dsp.eqHigh = dsp["eqHigh"] | 50;
  st.dsp.masterEQLow = dsp["masterEqLow"] | 50;
  st.dsp.masterEQMid = dsp["masterEqMid"] | 50;
  st.dsp.masterEQHigh = dsp["masterEqHi"] | 50;
  st.dsp.compThreshold = dsp["compThresh"] | 70;
  st.dsp.compRatio = dsp["compRatio"] | 40;
  st.dsp.saturation = dsp["sat"] | 0;
  st.dsp.limiterThresh = dsp["limiter"] | 95;

  // Канали
  JsonArrayConst channels = doc["channels"];
  for (int ch = 0; ch < MAX_CHANNELS && ch < (int)channels.size(); ch++) {
    JsonObjectConst c = channels[ch];
    st.channels[ch].sampleIndex = c["sample"] | 0;
    st.channels[ch].volume = c["vol"] | 80;
    st.channels[ch].pitch = c["pitch"] | 50;
    st.channels[ch].drive = c["drive"] | 0;
    st.channels[ch].start = c["start"] | 0;
    st.channels[ch].length = c["length"] | 100;
    st.channels[ch].attack = c["attack"] | 5;
    st.channels[ch].decay = c["decay"] | 50;
    st.channels[ch].pan = c["pan"] | 50;
    st.channels[ch].mute = c["mute"] | false;
    st.channels[ch].solo = c["solo"] | false;
  }

  // Паттерни (розпакування бітових масок)
  JsonArrayConst patterns = doc["patterns"];
  for (int p = 0; p < MAX_PATTERNS && p < (int)patterns.size(); p++) {
    JsonArrayConst pat = patterns[p];
    for (int ch = 0; ch < MAX_CHANNELS; ch++) {
      uint64_t maskLo = pat[ch * 2 + 0] | 0;
      uint64_t maskHi = pat[ch * 2 + 1] | 0;
      uint64_t mask = maskLo | (maskHi << 32);
      for (int s = 0; s < MAX_STEPS; s++) {
        st.patterns[p].steps[ch][s].active = (mask >> s) & 1;
      }
    }
    // Довжина паттерну — останній елемент масиву
    st.patterns[p].length = pat[MAX_CHANNELS * 2] | 16;
  }

  st.project.currentIndex = idx;
  return true;
}

// ── Публічне API ──────────────────────────────────────────────
bool storageSave() {
  if (!s_state || !s_state->project.sdAvailable)
    return false;
  const uint8_t idx = s_state->project.currentIndex;
  if (idx >= MAX_PROJECTS)
    return false;

  char path[48];
  buildPath(path, sizeof(path), s_state->project.names[idx]);

  // Видалити старий файл якщо існує (SD.open(WRITE) не очищає!)
  if (SD.exists(path))
    SD.remove(path);

  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.printf("[Storage] Не вдалося відкрити для запису: %s\n", path);
    return false;
  }

  bool ok = serializeState(file, *s_state);
  file.close();

  if (ok) {
    s_state->project.unsavedChanges = false;
    Serial.printf("[Storage] Збережено: %s\n", path);
  }
  return ok;
}

bool storageSaveAs(const char *name) {
  if (!s_state || !name || strlen(name) == 0)
    return false;
  const uint8_t idx = s_state->project.currentIndex;
  strlcpy(s_state->project.names[idx], name, PROJECT_NAME_LEN);
  return storageSave();
}

bool storageLoad(uint8_t index) {
  if (!s_state || !s_state->project.sdAvailable)
    return false;
  if (index >= s_state->project.numProjects)
    return false;

  char path[48];
  buildPath(path, sizeof(path), s_state->project.names[index]);

  File file = SD.open(path, FILE_READ);
  if (!file) {
    Serial.printf("[Storage] Не вдалося відкрити: %s\n", path);
    return false;
  }

  bool ok = deserializeState(file, *s_state, index);
  file.close();

  if (ok) {
    Serial.printf("[Storage] Завантажено: %s\n", path);
  }
  return ok;
}

bool storageDelete(uint8_t index) {
  if (!s_state || !s_state->project.sdAvailable)
    return false;
  if (index >= s_state->project.numProjects)
    return false;

  char path[48];
  buildPath(path, sizeof(path), s_state->project.names[index]);

  if (!SD.remove(path)) {
    Serial.printf("[Storage] Не вдалося видалити: %s\n", path);
    return false;
  }
  Serial.printf("[Storage] Видалено: %s\n", path);
  storageScanProjects(); // Оновити список
  return true;
}
