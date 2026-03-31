#include "samples.h"
#include "samples_data.h"
#include <Arduino.h>
#include <math.h>

// Built-in sample bank
const Sample builtin_samples[9] = {
    {kick_data, kick_data_length, "KICK", 36},
    {snare_data, snare_data_length, "SNARE", 38},
    {hihat_closed_data, hihat_closed_data_length, "HIHAT-C", 42},
    {hihat_open_data, hihat_open_data_length, "HIHAT-O", 46},
    {tom_data, tom_data_length, "TOM", 45},
    {clap_data, clap_data_length, "CLAP", 39},
    {crash_data, crash_data_length, "CRASH", 49},
    {perc_data, perc_data_length, "PERC", 56},
    {nullptr, 0, "EMPTY", 60} // Empty slot
};

// Sample players (8 channels + polyphony)
#define MAX_POLYPHONY 16
SamplePlayer players[MAX_POLYPHONY];

// EQ per channel (8 channels)
ChannelEQ channelEQ[8];

// Reverb and delay buffers
#define REVERB_BUFFER_SIZE 4096
#define DELAY_BUFFER_SIZE 8192
static int16_t reverbBuffer[REVERB_BUFFER_SIZE];
static int16_t delayBuffer[DELAY_BUFFER_SIZE];
static uint32_t reverbIndex = 0;
static uint32_t delayIndex = 0;

// Improved reverb - multiple delay lines
static int16_t reverbDelay1[2048];  // ~23ms
static int16_t reverbDelay2[3072];  // ~35ms  
static int16_t reverbDelay3[4096];  // ~46ms
static uint32_t reverbIdx1 = 0, reverbIdx2 = 0, reverbIdx3 = 0;
static float reverbFeedback = 0.85f;

// Sidechain compressor state
static float sidechainGain = 1.0f;
static float sidechainEnvelope = 0.0f;
static uint32_t sidechainReleaseCounter = 0;
#define SIDECHAIN_RELEASE_TIME 85000  // 85ms in microseconds (44.1kHz sample rate)
#define SIDECHAIN_RATIO 0.28f         // -11dB duck amount
#define SIDECHAIN_THRESHOLD 0.5f      // Trigger threshold

// Global audio parameters
float masterVolume = 0.8f;
float driveAmount = 0.0f;
float bitcrushAmount = 0.0f;
float reverbAmount = 0.0f;
float delayAmount = 0.0f;
float delayFeedback = 0.3f;

void initSampleEngine() {
    // Initialize sample players
    for(int i = 0; i < MAX_POLYPHONY; i++) {
        players[i].isActive = false;
        players[i].sample = nullptr;
        players[i].position = 0;
        players[i].channel = 0;
    }
    
    // Clear effect buffers
    memset(reverbBuffer, 0, sizeof(reverbBuffer));
    memset(delayBuffer, 0, sizeof(delayBuffer));
    memset(reverbDelay1, 0, sizeof(reverbDelay1));
    memset(reverbDelay2, 0, sizeof(reverbDelay2));
    memset(reverbDelay3, 0, sizeof(reverbDelay3));
    
    // Initialize sidechain
    sidechainGain = 1.0f;
    sidechainEnvelope = 0.0f;
    sidechainReleaseCounter = 0;
    
    // Initialize channel EQ
    initChannelEQ();
    
    Serial.println("🎵 Sample engine initialized");
}

// Channel classification for sidechain
bool isKickChannel(uint8_t channel) {
    // Channel 0 is typically kick in groovebox layout
    return channel == 0;
}

bool isBassChannel(uint8_t channel) {
    // Channel 4 is typically bass in groovebox layout  
    return channel == 4;
}

void triggerSample(uint8_t channel, uint8_t sampleIndex, float volume, float pitch) {
    // Find free player slot
    int freeSlot = -1;
    for(int i = 0; i < MAX_POLYPHONY; i++) {
        if(!players[i].isActive) {
            freeSlot = i;
            break;
        }
    }
    
    if(freeSlot == -1) {
        // Find oldest playing sample to replace
        uint32_t oldestPos = 0;
        for(int i = 0; i < MAX_POLYPHONY; i++) {
            if(players[i].isActive && players[i].position > oldestPos) {
                oldestPos = players[i].position;
                freeSlot = i;
            }
        }
    }
    
    if(freeSlot != -1 && sampleIndex < 9) {
        SamplePlayer& player = players[freeSlot];
        player.sample = &builtin_samples[sampleIndex];
        player.channel = channel;
        player.volume = volume;
        player.playbackRate = powf(2.0f, pitch / 12.0f); // Pitch in semitones
        player.position = 0;
        player.startPosition = 0;
        player.endPosition = player.sample->length;
        player.isActive = true;
        
        Serial.printf("Trigger: CH%d Sample %s Vol:%.2f Pitch:%.1f\n", 
                      channel, player.sample->name, volume, pitch);
    }
}

void processSamples(int16_t* outputBuffer, size_t bufferSize) {
    // Clear output buffer
    memset(outputBuffer, 0, bufferSize * sizeof(int16_t));
    
    // Track if kick triggered for sidechain
    bool kickTriggered = false;
    
    // Temporary buffers per channel for EQ processing
    static int16_t channelBuffers[8][512];
    memset(channelBuffers, 0, sizeof(channelBuffers));
    
    // Mix all active samples to per-channel buffers
    for(int i = 0; i < MAX_POLYPHONY; i++) {
        SamplePlayer& player = players[i];
        if(!player.isActive || !player.sample || !player.sample->data) continue;
        
        const int16_t* sampleData = player.sample->data;
        uint32_t sampleLength = player.sample->length;
        uint8_t ch = player.channel;
        
        // Check if this is a kick sample for sidechain trigger
        if(isKickChannel(ch) && player.position == 0) {
            kickTriggered = true;
        }
        
        for(size_t j = 0; j < bufferSize; j++) {
            if(player.position >= player.endPosition) {
                player.isActive = false;
                break;
            }
            
            // Linear interpolation for pitch
            float exactPos = player.position * player.playbackRate;
            uint32_t pos1 = (uint32_t)exactPos;
            uint32_t pos2 = pos1 + 1;
            float frac = exactPos - pos1;
            
            if(pos2 >= sampleLength) {
                int32_t sample = sampleData[pos1];
                sample = (int32_t)(sample * player.volume * masterVolume);
                
                // Apply sidechain to bass channels
                if(isBassChannel(ch)) {
                    sample = (int32_t)(sample * sidechainGain);
                }
                
                // Mix to channel buffer
                int32_t mixed = channelBuffers[ch][j] + sample;
                channelBuffers[ch][j] = (int16_t)constrain(mixed, -32768, 32767);
                player.position++;
                continue;
            }
            
            // Interpolate sample
            int32_t sample = sampleData[pos1] * (1.0f - frac) + sampleData[pos2] * frac;
            sample = (int32_t)(sample * player.volume * masterVolume);
            
            // Apply sidechain to bass channels
            if(isBassChannel(ch)) {
                sample = (int32_t)(sample * sidechainGain);
            }
            
            // Mix to channel buffer
            int32_t mixed = channelBuffers[ch][j] + sample;
            channelBuffers[ch][j] = (int16_t)constrain(mixed, -32768, 32767);
            
            player.position++;
        }
    }
    
    // Apply EQ to each channel and mix to output
    for(int ch = 0; ch < 8; ch++) {
        // Check if channel has active samples
        bool hasAudio = false;
        for(int i = 0; i < MAX_POLYPHONY; i++) {
            if(players[i].isActive && players[i].channel == ch) {
                hasAudio = true;
                break;
            }
        }
        
        if(hasAudio) {
            applyChannelEQ(channelBuffers[ch], bufferSize, ch);
            
            // Mix EQ'd channel to main output
            for(size_t j = 0; j < bufferSize; j++) {
                int32_t mixed = outputBuffer[j] + channelBuffers[ch][j];
                outputBuffer[j] = (int16_t)constrain(mixed, -32768, 32767);
            }
        }
    }
    
    // Trigger sidechain if kick was played
    if(kickTriggered) {
        triggerSidechain();
    }
    
    // Apply global DSP effects
    if(driveAmount > 0.0f) applyDrive(outputBuffer, bufferSize, driveAmount);
    if(bitcrushAmount > 0.0f) applyBitcrush(outputBuffer, bufferSize, bitcrushAmount);
    if(reverbAmount > 0.0f) applyReverb(outputBuffer, bufferSize, reverbAmount);
    if(delayAmount > 0.0f) applyDelay(outputBuffer, bufferSize, delayAmount, delayFeedback);
}

void stopAllSamples() {
    for(int i = 0; i < MAX_POLYPHONY; i++) {
        players[i].isActive = false;
    }
}

void stopChannelSamples(uint8_t channel) {
    for(int i = 0; i < MAX_POLYPHONY; i++) {
        if(players[i].channel == channel) {
            players[i].isActive = false;
        }
    }
}

// DSP Effects Implementation
void applyDrive(int16_t* buffer, size_t length, float amount) {
    if(amount <= 0.0f) return;
    
    // Multi-stage drive with different algorithms
    float drive = 1.0f + amount * 15.0f; // More drive range
    float mix = amount * 0.8f; // Wet/dry mix
    
    for(size_t i = 0; i < length; i++) {
        float sample = (float)buffer[i] / 32768.0f;
        
        // Stage 1: Soft saturation (tanh-based)
        float saturated = tanhf(sample * drive * 0.5f) * 2.0f;
        
        // Stage 2: Symmetrical clipping
        if(saturated > 0.4f) {
            saturated = 0.4f + (saturated - 0.4f) * 0.2f;
        } else if(saturated < -0.4f) {
            saturated = -0.4f + (saturated + 0.4f) * 0.2f;
        }
        
        // Stage 3: Hard clipping for aggressive tones
        if(saturated > 0.6f) saturated = 0.6f;
        if(saturated < -0.6f) saturated = -0.6f;
        
        // Mix dry/wet
        sample = sample * (1.0f - mix) + saturated * mix;
        
        // Apply slight high-pass to keep bass tight
        static float hpFilter = 0.0f;
        hpFilter = hpFilter * 0.95f + sample * 0.05f;
        sample = sample - hpFilter * 0.3f;
        
        buffer[i] = (int16_t)(sample * 32767.0f);
    }
}

void applyBitcrush(int16_t* buffer, size_t length, float amount) {
    // Reduce bit depth
    int bits = (int)(16.0f - amount * 12.0f); // 16 bits to 4 bits
    if(bits < 4) bits = 4;
    
    int steps = 1 << (16 - bits);
    int mask = ~(steps - 1);
    
    for(size_t i = 0; i < length; i++) {
        buffer[i] = buffer[i] & mask;
    }
}

void applyReverb(int16_t* buffer, size_t length, float amount) {
    for(size_t i = 0; i < length; i++) {
        // Multiple delay lines for richer reverb
        int16_t delay1 = reverbDelay1[reverbIdx1];
        int16_t delay2 = reverbDelay2[reverbIdx2];
        int16_t delay3 = reverbDelay3[reverbIdx3];
        
        // Combine delays with different feedback amounts
        int16_t reverbSignal = (delay1 + (delay2 >> 1) + (delay3 >> 2)) >> 1;
        
        // Apply feedback and low-pass filtering (simple averaging)
        int16_t feedbackSample = (buffer[i] + (int16_t)(reverbSignal * reverbFeedback)) >> 1;
        
        // Update delay lines
        reverbDelay1[reverbIdx1] = feedbackSample;
        reverbDelay2[reverbIdx2] = feedbackSample;
        reverbDelay3[reverbIdx3] = feedbackSample;
        
        // Increment indices
        reverbIdx1 = (reverbIdx1 + 1) % 2048;
        reverbIdx2 = (reverbIdx2 + 1) % 3072;
        reverbIdx3 = (reverbIdx3 + 1) % 4096;
        
        // Mix dry/wet with proper gain staging
        float wetGain = amount * 0.3f; // Reduce reverb gain to avoid clipping
        float dryGain = 1.0f - wetGain;
        buffer[i] = (int16_t)((buffer[i] * dryGain) + (reverbSignal * wetGain));
    }
}

void applyDelay(int16_t* buffer, size_t length, float amount, float feedback) {
    uint32_t delaySamples = (uint32_t)(SAMPLE_RATE * 0.25f); // 250ms delay
    
    for(size_t i = 0; i < length; i++) {
        int16_t delayed = delayBuffer[delayIndex];
        
        // Write current sample with feedback
        delayBuffer[delayIndex] = buffer[i] + (int16_t)(delayed * feedback);
        delayIndex = (delayIndex + 1) % DELAY_BUFFER_SIZE;
        
        // Mix dry/wet
        buffer[i] = (int16_t)((buffer[i] * (1.0f - amount)) + (delayed * amount));
    }
}

void triggerSidechain() {
    // Immediate duck when kick plays
    sidechainGain = SIDECHAIN_RATIO;
    sidechainEnvelope = 1.0f;
    sidechainReleaseCounter = SIDECHAIN_RELEASE_TIME;
}

void applySidechain(int16_t* buffer, size_t length, bool isKickChannel) {
    if(isKickChannel) {
        triggerSidechain();
    }
    
    for(size_t i = 0; i < length; i++) {
        // Apply current gain
        buffer[i] = (int16_t)(buffer[i] * sidechainGain);
        
        // Update release envelope
        if(sidechainReleaseCounter > 0) {
            sidechainReleaseCounter--;
            
            // Exponential release curve
            float releaseProgress = 1.0f - (float)sidechainReleaseCounter / SIDECHAIN_RELEASE_TIME;
            sidechainGain = SIDECHAIN_RATIO + (1.0f - SIDECHAIN_RATIO) * releaseProgress;
        } else {
            sidechainGain = 1.0f;
        }
    }
}

// EQ Implementation
void initChannelEQ() {
    for(int i = 0; i < 8; i++) {
        channelEQ[i].lowGain = 1.0f;
        channelEQ[i].midGain = 1.0f;
        channelEQ[i].highGain = 1.0f;
        
        channelEQ[i].lowZ1 = channelEQ[i].lowZ2 = 0.0f;
        channelEQ[i].midZ1 = channelEQ[i].midZ2 = 0.0f;
        channelEQ[i].highZ1 = channelEQ[i].highZ2 = 0.0f;
    }
}

void setChannelEQ(uint8_t channel, float low, float mid, float high) {
    if(channel >= 8) return;
    
    // Convert dB to gain (0-100 range maps to -12dB to +12dB)
    channelEQ[channel].lowGain = powf(10.0f, (low - 50.0f) * 0.24f / 20.0f);
    channelEQ[channel].midGain = powf(10.0f, (mid - 50.0f) * 0.24f / 20.0f);
    channelEQ[channel].highGain = powf(10.0f, (high - 50.0f) * 0.24f / 20.0f);
}

void applyChannelEQ(int16_t* buffer, size_t length, uint8_t channel) {
    if(channel >= 8) return;
    
    ChannelEQ& eq = channelEQ[channel];
    
    // Simple 3-band EQ using basic filters
    for(size_t i = 0; i < length; i++) {
        float sample = (float)buffer[i] / 32768.0f;
        
        // Low shelf filter (~200Hz crossover)
        float lowInput = sample;
        float lowOutput = lowInput + (eq.lowZ1 - lowInput) * 0.99f;
        eq.lowZ1 = lowOutput;
        float lowSignal = lowOutput * eq.lowGain;
        
        // High shelf filter (~2kHz crossover)  
        float highInput = sample - lowOutput;
        float highOutput = highInput + (eq.highZ1 - highInput) * 0.7f;
        eq.highZ1 = highOutput;
        float highSignal = highOutput * eq.highGain;
        
        // Mid signal (original - low - high)
        float midSignal = (sample - lowOutput - highOutput) * eq.midGain;
        
        // Combine bands
        float output = lowSignal + midSignal + highSignal;
        
        // Soft limiting
        if(output > 0.95f) output = 0.95f;
        if(output < -0.95f) output = -0.95f;
        
        buffer[i] = (int16_t)(output * 32767.0f);
    }
}
