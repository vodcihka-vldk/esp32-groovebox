#pragma once

#include <stdint.h>
#include <memory>

class AudioBuffer;
class DSPProcessor;

class AudioEngine {
public:
    struct Config {
        uint32_t sampleRate = 44100;
        uint16_t bufferSize = 512;
        uint8_t channels = 2;
    };
    
    AudioEngine(const Config& config);
    ~AudioEngine();
    
    bool initialize();
    void process();
    void shutdown();
    
    // Аудио обработка
    void processSamples(int16_t* outputBuffer, size_t bufferSize);
    void setInputBuffer(const int16_t* inputBuffer, size_t bufferSize);
    
    // Статус
    bool isInitialized() const { return m_initialized; }
    uint32_t getSampleRate() const { return m_config.sampleRate; }
    
private:
    Config m_config;
    bool m_initialized = false;
    
    // Внутренние компоненты
    std::unique_ptr<AudioBuffer> m_inputBuffer;
    std::unique_ptr<AudioBuffer> m_outputBuffer;
    std::unique_ptr<DSPProcessor> m_dspProcessor;
};
