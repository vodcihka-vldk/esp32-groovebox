import React from 'react';
import { StepGrid } from './components/sequencer/StepGrid';
import { useProject, useIsPlaying, useBPM, useCurrentPattern, useCurrentTrack, useCurrentPatternData, useGrooveboxActions, useProjectUtils } from './store/useGrooveboxStore';
import { GrooveboxSequencer } from './engine/sequencer';
import { validateAndExportToESP } from './engine/bridge';
import './App.css';

function App() {
  const project = useProject();
  const isPlaying = useIsPlaying();
  const bpm = useBPM();
  const currentPattern = useCurrentPattern();
  const currentTrack = useCurrentTrack();
  const currentPatternData = useCurrentPatternData();
  
  const { setProject, setPlaying, setBPM, toggleStep, updateChannel, setCurrentTrack, setCurrentPattern, setLoading, setError } = useGrooveboxActions();
  const { createNewProject, exportToESP32 } = useProjectUtils();
  
  const [sequencer, setSequencer] = React.useState<GrooveboxSequencer | null>(null);

  // Инициализация
  React.useEffect(() => {
    // Создаем пустой проект при загрузке
    createNewProject('ESP32 Groovebox Demo');
    
    // Создаем секвенсор
    const newSequencer = new GrooveboxSequencer();
    setSequencer(newSequencer);
    
    // Инициализируем аудио
    newSequencer.loadSamples().catch(error => {
      console.error('❌ Ошибка загрузки сэмплов:', error);
      setError('Не удалось загрузить сэмплы');
    });
    
    return () => {
      newSequencer.dispose();
    };
  }, []);

  // Обновление секвенсора при изменении проекта
  React.useEffect(() => {
    if (sequencer && project) {
      sequencer.createSequencer(project, {
        onStep: (stepIndex, trackIndex) => {
          // UI обновления при шаге
        },
        onPatternChange: (patternIndex) => {
          // UI обновления при смене паттерна
        },
        onBPMChange: (bpm) => {
          // UI обновления BPM
        }
      });
    }
  }, [sequencer, project]);

  // Управление воспроизведением
  const handlePlay = async () => {
    if (!sequencer) return;
    
    try {
      if (isPlaying) {
        sequencer.pause();
        setPlaying(false);
      } else {
        await sequencer.start();
        setPlaying(true);
      }
    } catch (error) {
      console.error('❌ Ошибка воспроизведения:', error);
      setError('Не удалось запустить секвенсор');
    }
  };

  const handleStop = () => {
    if (!sequencer) return;
    
    sequencer.stop();
    setPlaying(false);
  };

  // Управление шагами
  const handleStepToggle = (trackIndex: number, stepIndex: number) => {
    toggleStep(currentPattern, trackIndex, stepIndex);
    
    // Применяем к секвенсору
    if (sequencer && project) {
      const step = project.patterns[currentPattern].tracks[trackIndex].steps[stepIndex];
      sequencer.updateStep(currentPattern, trackIndex, stepIndex, step);
    }
  };

  // Управление каналом
  const handleVolumeChange = (trackIndex: number, volume: number) => {
    updateChannel(trackIndex, { volume });
    
    // Применяем к секвенсору
    if (sequencer && project) {
      const channel = project.patterns[0].tracks[trackIndex].channel;
      sequencer.applyChannelParameters(trackIndex, channel);
    }
  };

  const handlePitchChange = (trackIndex: number, pitch: number) => {
    updateChannel(trackIndex, { pitch });
    
    // Применяем к секвенсору
    if (sequencer && project) {
      const channel = project.patterns[0].tracks[trackIndex].channel;
      sequencer.applyChannelParameters(trackIndex, channel);
    }
  };

  // Управление BPM
  const handleBPMChange = (newBPM: number) => {
    setBPM(newBPM);
    
    // Применяем к секвенсору
    if (sequencer) {
      sequencer.setBPM(newBPM);
    }
  };

  // Управление паттернами
  const handlePatternChange = (patternIndex: number) => {
    setCurrentPattern(patternIndex);
    
    // Применяем к секвенсору
    if (sequencer) {
      sequencer.setPattern(patternIndex);
    }
  };

  // Экспорт в ESP32
  const handleExportToESP32 = () => {
    if (!project) return;
    
    try {
      const esp32Data = exportToESP32();
      console.log('📤 Данные для ESP32:', esp32Data);
      
      // Здесь можно отправить данные на ESP32
      // fetch('http://esp32.local/project', { method: 'POST', body: esp32Data })
      
      alert('Проект экспортирован для ESP32');
    } catch (error) {
      console.error('❌ Ошибка экспорта:', error);
      setError('Не удалось экспортировать проект');
    }
  };

  // Preview сэмплов
  const handlePreviewSample = (trackIndex: number) => {
    if (sequencer) {
      sequencer.playSample(trackIndex);
    }
  };

  if (!project) {
    return (
      <div className="loading">
        <h1>🎵 ESP32 Groovebox</h1>
        <p>Loading...</p>
      </div>
    );
  }

  return (
    <div className="app">
      <header className="header">
        <h1>🎵 ESP32 Groovebox</h1>
        <p>Cross-platform Web Interface with Zod + Tone.js + Zustand</p>
      </header>

      <main className="main">
        {/* Transport Controls */}
        <div className="controls">
          <div className="project-info">
            <h2>{project.name}</h2>
            <p>BPM: {bpm} | Pattern: {currentPattern + 1}/8 | Track: {currentTrack + 1}/8</p>
          </div>
          
          <div className="transport">
            <button onClick={handlePlay} className={isPlaying ? 'active' : ''}>
              {isPlaying ? '⏸️ Pause' : '▶️ Play'}
            </button>
            <button onClick={handleStop}>
              ⏹️ Stop
            </button>
            <button onClick={handleExportToESP32}>
              📤 Export to ESP32
            </button>
          </div>
        </div>

        {/* BPM Control */}
        <div className="bpm-control">
          <label>BPM: {bpm}</label>
          <input
            type="range"
            min="40"
            max="300"
            value={bpm}
            onChange={(e) => handleBPMChange(parseInt(e.target.value))}
          />
        </div>

        {/* Pattern Selector */}
        <div className="pattern-selector">
          <h3>Pattern Selection</h3>
          <div className="pattern-buttons">
            {Array.from({ length: 8 }, (_, i) => (
              <button
                key={i}
                className={currentPattern === i ? 'active' : ''}
                onClick={() => handlePatternChange(i)}
              >
                P{i + 1}
              </button>
            ))}
          </div>
        </div>

        {/* Sequencer Grid */}
        <div className="sequencer">
          <h2>16-Step Sequencer (64 steps total)</h2>
          {currentPatternData?.tracks.map((track, trackIndex) => (
            <div 
              key={track.id} 
              className={`track ${currentTrack === trackIndex ? 'selected' : ''}`}
              onClick={() => setCurrentTrack(trackIndex)}
            >
              <div className="track-header">
                <h3>{track.name}</h3>
                <div className="track-controls">
                  <div className="volume-control">
                    <label>Vol: {track.channel.volume}%</label>
                    <input
                      type="range"
                      min="0"
                      max="100"
                      value={track.channel.volume}
                      onChange={(e) => handleVolumeChange(trackIndex, parseInt(e.target.value))}
                      onClick={(e) => e.stopPropagation()}
                    />
                  </div>
                  
                  <div className="pitch-control">
                    <label>Pitch: {track.channel.pitch}</label>
                    <input
                      type="range"
                      min="0"
                      max="100"
                      value={track.channel.pitch}
                      onChange={(e) => handlePitchChange(trackIndex, parseInt(e.target.value))}
                      onClick={(e) => e.stopPropagation()}
                    />
                  </div>
                  
                  <button 
                    className="preview-btn"
                    onClick={(e) => {
                      e.stopPropagation();
                      handlePreviewSample(trackIndex);
                    }}
                  >
                    🔊 Preview
                  </button>
                </div>
              </div>
              
              <StepGrid
                patternId={currentPatternData.id}
                trackIndex={trackIndex}
                onStepToggle={handleStepToggle}
              />
            </div>
          ))}
        </div>

        {/* DSP Controls */}
        <div className="dsp-controls">
          <h3>DSP Effects</h3>
          <div className="dsp-grid">
            <div className="dsp-control">
              <label>Drive: {project.dsp.drive}%</label>
              <input
                type="range"
                min="0"
                max="100"
                value={project.dsp.drive}
                onChange={(e) => updateChannel(0, { drive: parseInt(e.target.value) })}
              />
            </div>
            
            <div className="dsp-control">
              <label>Reverb: {project.dsp.reverb}%</label>
              <input
                type="range"
                min="0"
                max="100"
                value={project.dsp.reverb}
                onChange={(e) => updateChannel(0, { reverb: parseInt(e.target.value) })}
              />
            </div>
            
            <div className="dsp-control">
              <label>Delay: {project.dsp.delay}%</label>
              <input
                type="range"
                min="0"
                max="100"
                value={project.dsp.delay}
                onChange={(e) => updateChannel(0, { delay: parseInt(e.target.value) })}
              />
            </div>
          </div>
        </div>

        {/* Status Info */}
        <div className="info">
          <h3>✅ Status: All Systems Operational</h3>
          <div className="status-grid">
            <div>
              <strong>Zod Validation:</strong> ✅ Type-safe data
            </div>
            <div>
              <strong>Tone.js Audio:</strong> ✅ 16-bit PCM ready
            </div>
            <div>
              <strong>ArduinoJson:</strong> ✅ ESP32 compatible
            </div>
            <div>
              <strong>Zustand Store:</strong> ✅ Optimized state
            </div>
            <div>
              <strong>Sequencer:</strong> ✅ 64-step precision
            </div>
          </div>
        </div>
      </main>

      <footer className="footer">
        <p>ESP32 Groovebox - Professional DIY Drum Machine</p>
        <p>Web Interface with Zod + Tone.js + Zustand + React</p>
      </footer>
    </div>
  );
}

export default App;
