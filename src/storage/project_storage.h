#pragma once

#include "state/groovebox_state.h"
#include <stdbool.h>
#include <stdint.h>

// ============================================================
//  Зберігання проектів на SD-карті
//  Формат файлів: /GROOVEBOX/<назва>.gbx (JSON)
//  Підтримує: ініціалізацію, сканування, збереження, завантаження
// ============================================================

// Ініціалізація SPI та SD-карти. Повертає true якщо успішно.
bool storageInit(GrooveboxState *state);

// Сканування SD-карти та заповнення project.names[]
void storageScanProjects();

// Зберегти поточний проект (за індексом state->project.currentIndex)
bool storageSave();

// Зберегти проект під вказаною назвою (якщо нова)
bool storageSaveAs(const char *name);

// Завантажити проект за індексом
bool storageLoad(uint8_t index);

// Видалити проект за індексом
bool storageDelete(uint8_t index);
