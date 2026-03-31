import { create } from 'zustand';
import { immer } from 'zustand/middleware/immer';
import React from 'react';
import { Project, Channel, Step, DSP, createEmptyProject } from '../../../shared/schemas';

/**
 * Zustand store с Immer для эффективного обновления состояния
 * Обеспечивает оптимизированное управление проектом Groovebox
 */

interface GrooveboxState {
  // Проект
  project: Project | null;
  
  // UI состояние
  isLoading: boolean;
  error: string | null;
  
  // Секвенсор состояние
  isPlaying: boolean;
  currentStep: number;
  currentPattern: number;
  currentTrack: number;
  bpm: number;
  
  // Actions
  setProject: (project: Project) => void;
  updateProject: (updates: Partial<Project>) => void;
  
  // Pattern actions
  setCurrentPattern: (patternIndex: number) => void;
  updateStep: (patternIndex: number, trackIndex: number, stepIndex: number, stepData: Partial<Step>) => void;
  toggleStep: (patternIndex: number, trackIndex: number, stepIndex: number) => void;
  
  // Track actions
  updateChannel: (trackIndex: number, channelUpdates: Partial<Channel>) => void;
  setCurrentTrack: (trackIndex: number) => void;
  
  // DSP actions
  updateDSP: (dspUpdates: Partial<DSP>) => void;
  
  // Transport actions
  setPlaying: (playing: boolean) => void;
  setCurrentStep: (step: number) => void;
  setBPM: (bpm: number) => void;
  
  // Utility actions
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
  clearError: () => void;
  reset: () => void;
}

export const useGrooveboxStore = create<GrooveboxState>()(
  immer((set) => ({
    // Initial state
    project: null,
    isLoading: false,
    error: null,
    isPlaying: false,
    currentStep: 0,
    currentPattern: 0,
    currentTrack: 0,
    bpm: 120,
    
    // Project actions
    setProject: (project: Project) => {
      set((state) => {
        state.project = project;
        state.bpm = project.bpm;
        state.error = null;
      });
    },
    
    updateProject: (updates: Partial<Project>) => {
      set((state) => {
        if (state.project) {
          Object.assign(state.project, updates);
        }
      });
    },
    
    // Pattern actions
    setCurrentPattern: (patternIndex: number) => {
      set((state) => {
        state.currentPattern = patternIndex;
        state.currentStep = 0;
      });
    },
    
    updateStep: (patternIndex: number, trackIndex: number, stepIndex: number, stepData: Partial<Step>) => {
      set((state) => {
        if (state.project) {
          const pattern = state.project.patterns[patternIndex];
          if (pattern && pattern.tracks[trackIndex]) {
            Object.assign(pattern.tracks[trackIndex].steps[stepIndex], stepData);
          }
        }
      });
    },
    
    toggleStep: (patternIndex: number, trackIndex: number, stepIndex: number) => {
      set((state) => {
        if (state.project) {
          const pattern = state.project.patterns[patternIndex];
          if (pattern && pattern.tracks[trackIndex]) {
            const step = pattern.tracks[trackIndex].steps[stepIndex];
            step.active = !step.active;
            
            // Устанавливаем velocity по умолчанию для активных шагов
            if (step.active && !step.velocity) {
              step.velocity = 100;
            }
          }
        }
      });
    },
    
    // Track actions
    updateChannel: (trackIndex: number, channelUpdates: Partial<Channel>) => {
      set((state) => {
        if (state.project) {
          // Обновляем все паттерны (каналы общие для всех паттернов)
          state.project.patterns.forEach((pattern: any) => {
            if (pattern.tracks[trackIndex]) {
              Object.assign(pattern.tracks[trackIndex].channel, channelUpdates);
            }
          });
          
          // Обновляем микшер
          if (state.project.mixer.channels[trackIndex]) {
            Object.assign(state.project.mixer.channels[trackIndex], channelUpdates);
          }
        }
      });
    },
    
    setCurrentTrack: (trackIndex: number) => {
      set((state) => {
        state.currentTrack = trackIndex;
      });
    },
    
    // DSP actions
    updateDSP: (dspUpdates: Partial<DSP>) => {
      set((state) => {
        if (state.project) {
          Object.assign(state.project.dsp, dspUpdates);
        }
      });
    },
    
    // Transport actions
    setPlaying: (playing: boolean) => {
      set((state) => {
        state.isPlaying = playing;
      });
    },
    
    setCurrentStep: (step: number) => {
      set((state) => {
        state.currentStep = step;
      });
    },
    
    setBPM: (bpm: number) => {
      set((state) => {
        const validBPM = Math.max(40, Math.min(300, bpm));
        state.bpm = validBPM;
        if (state.project) {
          state.project.bpm = validBPM;
        }
      });
    },
    
    // Utility actions
    setLoading: (loading: boolean) => {
      set((state) => {
        state.isLoading = loading;
      });
    },
    
    setError: (error: string | null) => {
      set((state) => {
        state.error = error;
      });
    },
    
    clearError: () => {
      set((state) => {
        state.error = null;
      });
    },
    
    reset: () => {
      set((state) => {
        state.project = null;
        state.isLoading = false;
        state.error = null;
        state.isPlaying = false;
        state.currentStep = 0;
        state.currentPattern = 0;
        state.currentTrack = 0;
        state.bpm = 120;
      });
    }
  }))
);

/**
 * Selectors для оптимизации ререндеров
 */
export const useProject = () => useGrooveboxStore((state) => state.project);
export const useIsPlaying = () => useGrooveboxStore((state) => state.isPlaying);
export const useCurrentStep = () => useGrooveboxStore((state) => state.currentStep);
export const useCurrentPattern = () => useGrooveboxStore((state) => state.currentPattern);
export const useCurrentTrack = () => useGrooveboxStore((state) => state.currentTrack);
export const useBPM = () => useGrooveboxStore((state) => state.bpm);
export const useLoading = () => useGrooveboxStore((state) => state.isLoading);
export const useError = () => useGrooveboxStore((state) => state.error);

/**
 * Комбинированные selectors
 */
export const useCurrentPatternData = () => {
  const project = useProject();
  const currentPattern = useCurrentPattern();
  
  return project?.patterns[currentPattern] || null;
};

export const useCurrentTrackData = () => {
  const currentPatternData = useCurrentPatternData();
  const currentTrack = useCurrentTrack();
  
  return currentPatternData?.tracks[currentTrack] || null;
};

export const useChannelData = (trackIndex: number) => {
  const project = useProject();
  
  return project?.patterns[0]?.tracks[trackIndex]?.channel || null;
};

/**
 * Actions hooks
 */
export const useGrooveboxActions = () => {
  const store = useGrooveboxStore();
  
  return {
    setProject: store.setProject,
    updateProject: store.updateProject,
    setCurrentPattern: store.setCurrentPattern,
    updateStep: store.updateStep,
    toggleStep: store.toggleStep,
    updateChannel: store.updateChannel,
    setCurrentTrack: store.setCurrentTrack,
    updateDSP: store.updateDSP,
    setPlaying: store.setPlaying,
    setCurrentStep: store.setCurrentStep,
    setBPM: store.setBPM,
    setLoading: store.setLoading,
    setError: store.setError,
    clearError: store.clearError,
    reset: store.reset
  };
};

/**
 * Утилиты для работы с проектом
 */
export const useProjectUtils = () => {
  const project = useProject();
  const actions = useGrooveboxActions();
  
  const createNewProject = React.useCallback((name: string = 'New Beat') => {
    const newProject = createEmptyProject(name);
    actions.setProject(newProject);
  }, [actions]);
  
  const exportToESP32 = React.useCallback(() => {
    if (!project) return null;
    
    // Zod валидация и конвертация
    const arduinoFormat = {
      name: project.name.substring(0, 16),
      bpm: project.bpm,
      masterVolume: project.masterVolume,
      patterns: project.patterns.map(pattern =>
        pattern.tracks.map(track =>
          track.steps.map(step => step.active)
        )
      ),
      channels: project.patterns[0].tracks.map(track => track.channel),
      dsp: project.dsp
    };
    
    return JSON.stringify(arduinoFormat, null, 2);
  }, [project]);
  
  const importFromESP32 = React.useCallback((jsonString: string) => {
    try {
      actions.setLoading(true);
      const data = JSON.parse(jsonString);
      
      const importedProject: Project = {
        version: "1.0",
        name: data.name || "Imported Project",
        bpm: Math.max(40, Math.min(300, data.bpm || 120)),
        masterVolume: Math.max(0, Math.min(100, data.masterVolume || 80)),
        swing: 50,
        patterns: Array.from({ length: 8 }, (_, pIndex) => ({
          id: `pattern_${pIndex}`,
          name: `Pattern ${pIndex + 1}`,
          length: '64',
          swing: 50,
          tracks: Array.from({ length: 8 }, (_, tIndex) => ({
            id: `track_${tIndex}`,
            name: `Track ${tIndex + 1}`,
            sampleId: `sample_${data.channels[tIndex]?.sample || 0}`,
            channel: data.channels[tIndex] || {
              volume: 80,
              pitch: 50,
              sample: 0,
              drive: 0,
              start: 0,
              length: 100,
              attack: 5,
              decay: 50
            },
            steps: Array.from({ length: 64 }, (_, sIndex) => ({
              active: Boolean(data.patterns[pIndex]?.[tIndex]?.[sIndex])
            })),
            parameterLocks: []
          }))
        })),
        mixer: {
          channels: Array.from({ length: 8 }, (_, index) => ({
            id: `track_${index}`,
            volume: data.channels[index]?.volume || 80,
            pan: 0,
            mute: false,
            solo: false
          }))
        },
        dsp: data.dsp || {
          drive: 0,
          reverb: 0,
          delay: 0,
          bitcrush: 0,
          eqLow: 0,
          eqMid: 0,
          eqHigh: 0
        },
        metadata: {
          createdAt: new Date().toISOString(),
          updatedAt: new Date().toISOString()
        }
      };
      
      actions.setProject(importedProject);
      actions.setError(null);
    } catch (error) {
      actions.setError(`Ошибка импорта: ${error}`);
    } finally {
      actions.setLoading(false);
    }
  }, [actions]);
  
  return {
    createNewProject,
    exportToESP32,
    importFromESP32
  };
};
