// High-quality generated samples for ESP32 Groovebox
// Generated with numpy synthesis - professional drum sounds

#include <stdint.h>

// Kick drum sample (frequency sweep 150Hz -> 40Hz, exponential decay)
const uint16_t kick_data_length = 22050; // 0.5 seconds at 44.1kHz
const int16_t kick_data[] = {
    #include "kick_data_new.inc"
};

// Snare drum sample (250Hz tone + noise, fast decay)
const uint16_t snare_data_length = 8820; // 0.2 seconds at 44.1kHz
const int16_t snare_data[] = {
    #include "snare_data_new.inc"
};

// Hi-hat closed sample (high-frequency noise, fast decay)
const uint16_t hihat_closed_data_length = 4410; // 0.1 seconds at 44.1kHz
const int16_t hihat_closed_data[] = {
    #include "hihat_closed_data_new.inc"
};

// Hi-hat open sample (high-frequency noise, slow decay)
const uint16_t hihat_open_data_length = 4410; // 0.1 seconds at 44.1kHz
const int16_t hihat_open_data[] = {
    #include "hihat_open_data_new.inc"
};

// Tom sample (frequency sweep 200Hz -> 50Hz, medium decay)
const uint16_t tom_data_length = 17640; // 0.4 seconds at 44.1kHz
const int16_t tom_data[] = {
    #include "tom_data_new.inc"
};

// Clap sample (multiple noise bursts for clap effect)
const uint16_t clap_data_length = 13230; // 0.3 seconds at 44.1kHz
const int16_t clap_data[] = {
    #include "clap_data_new.inc"
};

// Crash cymbal sample (metallic noise + harmonics, slow decay)
const uint16_t crash_data_length = 66150; // 1.5 seconds at 44.1kHz
const int16_t crash_data[] = {
    #include "crash_data_new.inc"
};

// Percussion sample (synthesized)
const uint16_t perc_data_length = 8820; // 0.2 seconds at 44.1kHz
const int16_t perc_data[] = {
    // Generated percussion - filtered noise burst
    #include "perc_data.inc"
};
