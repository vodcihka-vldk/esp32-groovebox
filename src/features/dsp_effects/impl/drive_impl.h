#pragma once

#include "dsp_effects_api.h"

class DriveImpl : public DSPEffectsAPI {
public:
    DriveImpl();
    
    void setDriveAmount(float amount) override;
    float getDriveAmount() const override;
    
    void setReverbAmount(float amount) override { /* не реализовано */ }
    float getReverbAmount() const override { return 0.0f; }
    
    void setDelayTime(float time) override { /* не реализовано */ }
    float getDelayTime() const override { return 0.0f; }
    
    void setBitcrushAmount(float amount) override { /* не реализовано */ }
    float getBitcrushAmount() const override { return 0.0f; }
    
    void setEQLow(float gain) override { /* не реализовано */ }
    void setEQMid(float gain) override { /* не реализовано */ }
    void setEQHigh(float gain) override { /* не реализовано */ }
    
    void processEffects(int16_t* buffer, size_t length) override;
    
private:
    float m_driveAmount = 0.0f;
    
    void applyDrive(int16_t* buffer, size_t length);
};
