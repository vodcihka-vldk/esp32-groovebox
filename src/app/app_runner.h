#pragma once

#include <memory>
#include "entities/audio/model/audio_engine.h"
#include "entities/sequencer/model/pattern.h"
#include "features/dsp_effects/api/dsp_effects_api.h"
#include "features/dsp_effects/impl/drive_impl.h"

class AppRunner {
public:
    static AppRunner& getInstance();
    
    bool initialize();
    void run();
    void shutdown();
    
    // Доступ к компонентам
    AudioEngine* getAudioEngine() const { return m_audioEngine.get(); }
    DSPEffectsAPI* getDSPEffects() const { return m_dspEffects.get(); }
    
private:
    AppRunner() = default;
    
    // Компоненты
    std::unique_ptr<AudioEngine> m_audioEngine;
    std::unique_ptr<DSPEffectsAPI> m_dspEffects;
    
    // Конфигурации
    AudioEngine::Config m_audioConfig;
    
    // Инициализация
    bool setupAudioEngine();
    bool setupDSPEffects();
    bool setupFreeRTOSTasks();
    
    // Состояние
    bool m_initialized = false;
};
