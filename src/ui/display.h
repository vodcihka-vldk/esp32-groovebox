#pragma once

#include "state/groovebox_state.h"

// ============================================================
//  Модуль відображення (LCD 16×2)
//  Відповідає за весь рендеринг інтерфейсу.
//  Викликається тільки з UI-задачі.
// ============================================================

// Ініціалізація LCD (I2C через Wire)
void displayInit();

// Відобразити заставку (splash-screen)
void displayShowSplash(const char *line1, const char *line2);

// Головна функція рендерингу — обирає екран за станом
void displayRender(const GrooveboxState *state);
