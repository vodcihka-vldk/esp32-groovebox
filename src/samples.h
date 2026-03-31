#ifndef SAMPLES_H
#define SAMPLES_H

#include <stdint.h>

// Built-in sample data declarations
extern const uint16_t kick_data_length;
extern const int16_t kick_data[];

extern const uint16_t snare_data_length;
extern const int16_t snare_data[];

extern const uint16_t hihat_closed_data_length;
extern const int16_t hihat_closed_data[];

extern const uint16_t hihat_open_data_length;
extern const int16_t hihat_open_data[];

extern const uint16_t tom_data_length;
extern const int16_t tom_data[];

extern const uint16_t clap_data_length;
extern const int16_t clap_data[];

extern const uint16_t crash_data_length;
extern const int16_t crash_data[];

extern const uint16_t perc_data_length;
extern const int16_t perc_data[];

// Sample bank structure
struct Sample {
    const int16_t* data;
    uint32_t length;
    const char* name;
    uint8_t baseNote; // MIDI note number (60 = C4)
};

// Built-in sample bank
extern const Sample builtin_samples[9];

// Sample playback state
struct SamplePlayer {
    const Sample* sample;
    uint32_t position;
    uint32_t startPosition;
    uint32_t endPosition;
    float playbackRate;
    float volume;
    bool isActive;
    uint8_t channel;
};

// 3-band EQ per channel
struct ChannelEQ {
    float lowGain;   // -12dB to +12dB
    float midGain;   // -12dB to +12dB  
    float highGain;  // -12dB to +12dB
    
    // Simple biquad filter states
    float lowZ1, lowZ2;
    float midZ1, midZ2;
    float highZ1, highZ2;
};

// Audio processing functions
void initSampleEngine();
void triggerSample(uint8_t channel, uint8_t sampleIndex, float volume = 1.0f, float pitch = 0.0f);
void processSamples(int16_t* outputBuffer, size_t bufferSize);
void stopAllSamples();
void stopChannelSamples(uint8_t channel);

// DSP Effects
void applyDrive(int16_t* buffer, size_t length, float amount);
void applyBitcrush(int16_t* buffer, size_t length, float amount);
void applyReverb(int16_t* buffer, size_t length, float amount);
void applyDelay(int16_t* buffer, size_t length, float amount, float feedback);
void applySidechain(int16_t* buffer, size_t length, bool isKickChannel);
void triggerSidechain();

// EQ functions
void initChannelEQ();
void applyChannelEQ(int16_t* buffer, size_t length, uint8_t channel);
void setChannelEQ(uint8_t channel, float low, float mid, float high);

#endif // SAMPLES_H
