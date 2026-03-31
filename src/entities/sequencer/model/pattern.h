#pragma once

#include <string>
#include <array>

class Pattern {
public:
    static constexpr uint8_t k_MAX_TRACKS = 8;
    static constexpr uint8_t k_MAX_STEPS = 64;
    
    struct Step {
        bool active = false;
        uint8_t velocity = 127;
        uint8_t note = 60;
        uint8_t length = 16;
    };
    
    Pattern();
    explicit Pattern(const std::string& name);
    
    // Управление паттерном
    void setName(const std::string& name);
    const std::string& getName() const;
    
    // Управление шагами
    void setStep(uint8_t track, uint8_t step, const Step& stepData);
    Step getStep(uint8_t track, uint8_t step) const;
    void clearStep(uint8_t track, uint8_t step);
    void clearTrack(uint8_t track);
    void clear();
    
    // Статус
    bool isEmpty() const;
    uint8_t getActiveStepsCount(uint8_t track) const;
    
private:
    std::string m_name;
    std::array<std::array<Step, k_MAX_STEPS>, k_MAX_TRACKS> m_steps;
};
