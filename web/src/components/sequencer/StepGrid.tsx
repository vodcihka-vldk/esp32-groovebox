import React, { useState, useEffect } from 'react';
import { useSequencer } from '../hooks/useSequencer';
import { Step } from '../../../shared/schemas';

interface StepGridProps {
  patternId: string;
  trackIndex: number;
}

export const StepGrid: React.FC<StepGridProps> = ({ patternId, trackIndex }) => {
  const { getPattern, setStep, getStep } = useSequencer();
  const [steps, setSteps] = useState<Step[]>([]);
  const [currentStep, setCurrentStep] = useState(0);
  
  useEffect(() => {
    const pattern = getPattern(patternId);
    if (pattern) {
      setSteps(pattern.tracks[trackIndex].steps);
    }
  }, [patternId, trackIndex, getPattern]);
  
  useEffect(() => {
    // Подписка на события секвенсора
    const unsubscribe = useSequencer().onStep((data) => {
      if (data.patternId === patternId) {
        setCurrentStep(data.step);
      }
    });
    
    return unsubscribe;
  }, [patternId]);
  
  const handleStepClick = (stepIndex: number) => {
    const newStep = { ...steps[stepIndex], active: !steps[stepIndex].active };
    setStep(patternId, trackIndex, stepIndex, newStep);
    setSteps(prev => {
      const newSteps = [...prev];
      newSteps[stepIndex] = newStep;
      return newSteps;
    });
  };
  
  const handleVelocityChange = (stepIndex: number, velocity: number) => {
    const newStep = { ...steps[stepIndex], velocity };
    setStep(patternId, trackIndex, stepIndex, newStep);
    setSteps(prev => {
      const newSteps = [...prev];
      newSteps[stepIndex] = newStep;
      return newSteps;
    });
  };
  
  return (
    <div className="step-grid">
      <div className="step-numbers">
        {Array.from({ length: 16 }, (_, i) => (
          <div key={i} className="step-number">
            {i + 1}
          </div>
        ))}
      </div>
      
      <div className="step-buttons">
        {steps.slice(0, 16).map((step, index) => (
          <div key={index} className="step-container">
            <button
              className={`step-button ${step.active ? 'active' : ''} ${currentStep === index ? 'current' : ''}`}
              onClick={() => handleStepClick(index)}
              style={{
                backgroundColor: step.active 
                  ? `rgba(59, 130, 246, ${step.velocity / 127})` 
                  : '#374151'
              }}
            >
              {step.active && (
                <div className="velocity-indicator" style={{ height: `${(step.velocity / 127) * 100}%` }} />
              )}
            </button>
            
            <input
              type="range"
              min="0"
              max="127"
              value={step.velocity}
              onChange={(e) => handleVelocityChange(index, parseInt(e.target.value))}
              className="velocity-slider"
              style={{ opacity: step.active ? 1 : 0.3 }}
            />
          </div>
        ))}
      </div>
      
      <style jsx>{`
        .step-grid {
          display: flex;
          gap: 8px;
          padding: 16px;
          background: #1f2937;
          border-radius: 8px;
        }
        
        .step-numbers {
          display: flex;
          flex-direction: column;
          gap: 4px;
          margin-right: 8px;
        }
        
        .step-number {
          width: 20px;
          height: 20px;
          display: flex;
          align-items: center;
          justify-content: center;
          color: #9ca3af;
          font-size: 10px;
        }
        
        .step-buttons {
          display: flex;
          gap: 4px;
        }
        
        .step-container {
          display: flex;
          flex-direction: column;
          gap: 2px;
        }
        
        .step-button {
          width: 32px;
          height: 32px;
          border: 1px solid #4b5563;
          border-radius: 4px;
          background: #374151;
          cursor: pointer;
          position: relative;
          transition: all 0.2s;
        }
        
        .step-button:hover {
          border-color: #6b7280;
        }
        
        .step-button.active {
          border-color: #3b82f6;
          box-shadow: 0 0 8px rgba(59, 130, 246, 0.5);
        }
        
        .step-button.current {
          border-color: #10b981;
          box-shadow: 0 0 12px rgba(16, 185, 129, 0.7);
        }
        
        .velocity-indicator {
          position: absolute;
          bottom: 0;
          left: 0;
          right: 0;
          background: #3b82f6;
          border-radius: 0 0 2px 2px;
        }
        
        .velocity-slider {
          width: 32px;
          height: 8px;
          -webkit-appearance: none;
          appearance: none;
          background: #4b5563;
          border-radius: 2px;
          outline: none;
        }
        
        .velocity-slider::-webkit-slider-thumb {
          -webkit-appearance: none;
          appearance: none;
          width: 8px;
          height: 8px;
          background: #3b82f6;
          border-radius: 50%;
          cursor: pointer;
        }
      `}</style>
    </div>
  );
};
