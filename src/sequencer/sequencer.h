#pragma once

#include "state/groovebox_state.h"
#include <stdbool.h>
#include <stdint.h>

// ============================================================
//  Секвенсор Groovebox
//  Відповідає за:
//   - відлік кроків з точним BPM-таймером (мкс)
//   - тригер семплів через sample_engine
//   - застосування параметричних локів
//   - свінг (swing) з альтернуючим зсувом парних кроків
// ============================================================

// ── API ───────────────────────────────────────────────────────

// Ініціалізація: прив'язати до глобального стану
void seqInit(GrooveboxState *state);

// FreeRTOS-задача (нескінченний цикл, Core 1)
void seqTask(void *param);

// Управління
void seqPlay();
void seqStop();
void seqToggle();
bool seqIsPlaying();

// Примусовий перехід на крок (для тест-режиму)
void seqJumpToStep(uint8_t step);

// Поточний крок (для відображення на UI)
uint8_t seqCurrentStep();
