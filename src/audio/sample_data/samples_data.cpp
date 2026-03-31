// ============================================================
//  Визначення (definitions) масивів PCM-даних для семплів
//  Включаємо .inc файли тут, щоб компілятор бачив їх тільки
//  в одній одиниці трансляції — уникаємо ODR violation.
// ============================================================

#include "samples_data.h"
#include <stdint.h>

// Бас-барабан — 22050 зразків (0.5 с @ 44100 Гц)
const uint32_t kick_data_length = 22050;
const int16_t kick_data[] = {
#include "../../kick_data_new.inc"
};

// Малий барабан — 8820 зразків (0.2 с)
const uint32_t snare_data_length = 8820;
const int16_t snare_data[] = {
#include "../../snare_data_new.inc"
};

// Закритий хай-хет — 4410 зразків (0.1 с)
const uint32_t hihat_closed_data_length = 4410;
const int16_t hihat_closed_data[] = {
#include "../../hihat_closed_data_new.inc"
};

// Відкритий хай-хет — 4410 зразків (0.1 с)
const uint32_t hihat_open_data_length = 4410;
const int16_t hihat_open_data[] = {
#include "../../hihat_open_data_new.inc"
};

// Том-том — 17640 зразків (0.4 с)
const uint32_t tom_data_length = 17640;
const int16_t tom_data[] = {
#include "../../tom_data_new.inc"
};

// Клап — 13230 зразків (0.3 с)
const uint32_t clap_data_length = 13230;
const int16_t clap_data[] = {
#include "../../clap_data_new.inc"
};

// Тарілка — 66150 зразків (1.5 с)
const uint32_t crash_data_length = 66150;
const int16_t crash_data[] = {
#include "../../crash_data_new.inc"
};
