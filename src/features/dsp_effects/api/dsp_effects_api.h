#pragma once

#include <stdint.h>

class DSPEffectsAPI {
public:
    virtual ~DSPEffectsAPI() = default;
    
    // Drive эффект
    virtual void setDriveAmount(float amount) = 0;
    virtual float getDriveAmount() const = 0;
    
    // Reverb эффект
    virtual void setReverbAmount(float amount) = 0;
    virtual void setReverbSize(float size) = 0;
    virtual float getReverbAmount() const = 0;
    
    // Delay эффект
    virtual void setDelayTime(float time) = 0;
    virtual void setDelayFeedback(float feedback) = 0;
    virtual float getDelayTime() const = 0;
    
    // Bitcrush эффект
    virtual void setBitcrushAmount(float amount) = 0;
    virtual float getBitcrushAmount() const = 0;
    
    // EQ эффект
    virtual void setEQLow(float gain) = 0;
    virtual void setEQMid(float gain) = 0;
    virtual void setEQHigh(float gain) = 0;
    
    // Применение эффектов
    virtual void processEffects(int16_t* buffer, size_t length) = 0;
};
