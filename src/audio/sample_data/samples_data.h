// ============================================================
//  Дані вбудованих семплів Groovebox
//  Усі семпли — 16-бітний PCM, 44100 Гц, моно
//
//  Цей файл є ЄДИНИМ місцем включення .inc файлів.
//  Визначення (definition) йдуть тут, а не у .h заголовку.
//  Зовнішні модулі отримують доступ через sample_engine.h
// ============================================================

#pragma once

#include <stdint.h>

// ── Бас-барабан (kick): 150→40 Гц, 0.5 с ─────────────────────
extern const uint32_t kick_data_length;
extern const int16_t kick_data[];

// ── Малий барабан (snare): тон + шум, 0.2 с ──────────────────
extern const uint32_t snare_data_length;
extern const int16_t snare_data[];

// ── Закритий хай-хет (hihat-c): шум ВЧ, 0.1 с ───────────────
extern const uint32_t hihat_closed_data_length;
extern const int16_t hihat_closed_data[];

// ── Відкритий хай-хет (hihat-o): шум ВЧ, повільний спад ──────
extern const uint32_t hihat_open_data_length;
extern const int16_t hihat_open_data[];

// ── Том-том: 200→50 Гц, 0.4 с ────────────────────────────────
extern const uint32_t tom_data_length;
extern const int16_t tom_data[];

// ── Клап (clap): серія імпульсів, 0.3 с ─────────────────────
extern const uint32_t clap_data_length;
extern const int16_t clap_data[];

// ── Тарілка (crash): метал + гармоніки, 1.5 с ────────────────
extern const uint32_t crash_data_length;
extern const int16_t crash_data[];
