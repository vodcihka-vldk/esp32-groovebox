#pragma once

#include "config/hardware_config.h"
#include <stdbool.h>
#include <stdint.h>

// ============================================================
//  Центральний стан Groovebox
//  Єдина структура, що описує весь поточний стан пристрою.
//  Доступ з будь-якого модуля через вказівник або посилання.
// ============================================================

// ── Параметри одного каналу ───────────────────────────────────
struct ChannelState {
  uint8_t sampleIndex = 0; // Індекс вбудованого семпла (0-8)
  uint8_t volume = 80;     // Гучність каналу (0-100)
  uint8_t pitch = 50;      // Тон (0-100, 50 = оригінал)
  uint8_t drive = 0;       // Дисторшн каналу (0-100)
  uint8_t start = 0;       // Початок семпла (0-100 %)
  uint8_t length = 100; // Довжина семпла (0-100 %)
  uint8_t attack = 5;   // Атака огинаючої (0-100)
  uint8_t decay = 50;   // Спад огинаючої (0-100)
  uint8_t pan = 50;  // Панорама (0=лів, 50=центр, 100=прав)
  bool mute = false; // Заглушити канал
  bool solo = false; // Сольо-режим
};

// ── Параметри DSP ефектів (глобальні) ────────────────────────
struct DSPState {
  uint8_t driveAmount = 0;    // Дисторшн (0-100)
  uint8_t reverbAmount = 0;   // Реверберація (0-100)
  uint8_t delayAmount = 0;    // Затримка (0-100)
  uint8_t delayFeedback = 30; // Зворотній зв'язок delay (0-100)
  uint8_t bitcrushAmount = 0; // Бітдавлення (0-100)
  uint8_t eqLow = 50;         // НЧ EQ (0-100, 50=flat)
  uint8_t eqMid = 50;         // СЧ EQ (0-100, 50=flat)
  uint8_t eqHigh = 50;        // ВЧ EQ (0-100, 50=flat)

  // ── Мастер-ланцюг ─────────────────────────────────────────
  uint8_t masterEQLow = 50;
  uint8_t masterEQMid = 50;
  uint8_t masterEQHigh = 50;
  uint8_t compThreshold = 70; // Поріг компресора (0-100)
  uint8_t compRatio = 40;     // Ступінь компресії (0-100)
  uint8_t compAttack = 20;
  uint8_t compRelease = 50;
  uint8_t saturation = 0;     // Насичення (0-100)
  uint8_t limiterThresh = 95; // Поріг лімітера (0-100)
};

// ── Паттерн: крок паттерна ────────────────────────────────────
struct Step {
  bool active = false;    // Крок активний?
  uint8_t velocity = 100; // Динаміка (0-127)
  // Параметричні локи (відносний зсув від параметра каналу)
  bool hasParamLock = false;
  uint8_t lockVolume = 0xFF; // 0xFF = не заблоковано
  uint8_t lockPitch = 0xFF;
};

// ── Паттерн: MAX_CHANNELS × MAX_STEPS кроків ─────────────────
struct Pattern {
  Step steps[MAX_CHANNELS][MAX_STEPS];
  uint8_t length = 16; // Активна кількість кроків (1-64)
};

// ── Стан секвенсора ───────────────────────────────────────────
struct SequencerState {
  bool isPlaying = false;
  uint8_t currentStep = 0;
  uint8_t currentPat = 0; // Активний паттерн
  uint16_t bpm = BPM_DEFAULT;
  uint8_t swing = 0; // Свінг (0-100)
};

// ── Стан проекту ──────────────────────────────────────────────
struct ProjectState {
  uint8_t currentIndex = 0;
  uint8_t numProjects = 0;
  char names[MAX_PROJECTS][PROJECT_NAME_LEN];
  bool sdAvailable = false;
  bool unsavedChanges = false;
};

// ── Стан UI ───────────────────────────────────────────────────
enum class UIScreen : uint8_t {
  MAIN,
  CHANNEL,
  PATTERN,
  DSP,
  SOUND,
  MIXER,
  SEQUENCER,
  PERFORMANCE,
  SYSTEM,
  PARAM_LOCK,
  MASTERING
};

struct UIState {
  UIScreen screen = UIScreen::MAIN;
  uint8_t cursor = 0; // Поточна позиція курсору у меню
  uint8_t selChannel = 0;  // Виділений канал
  bool needsRedraw = true; // Прапор перемалювання
};

// ── Головна структура стану ───────────────────────────────────
struct GrooveboxState {
  SequencerState seq;
  ProjectState project;
  UIState ui;
  DSPState dsp;

  Pattern patterns[MAX_PATTERNS];
  ChannelState channels[MAX_CHANNELS];

  uint8_t masterVolume = 80;

  // Параметричні локи (канал × крок × параметр)
  // Зберігається окремо для швидкого доступу
  bool paramLocks[MAX_CHANNELS][MAX_STEPS][8] = {};
};

// ── Глобальний доступ (Singleton через extern) ────────────────
// Визначається в main.cpp, доступний скрізь через extern
extern GrooveboxState g_state;
