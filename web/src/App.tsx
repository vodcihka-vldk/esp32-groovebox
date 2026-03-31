import React from 'react';
import { StepGrid } from './components/sequencer/StepGrid';
import { useProjectManager } from './hooks/useProjectManager';
import { AudioService } from './services/AudioService';
import { createEmptyProject } from '../../shared/schemas';
import './App.css';

function App() {
  const { project, updateChannel, updateStepData, createNewProject } = useProjectManager();
  const [audioService] = React.useState(() => new AudioService());

  React.useEffect(() => {
    // Создаем пустой проект при загрузке
    createNewProject('ESP32 Groovebox Demo');
    
    // Инициализируем аудио сервис
    audioService.loadSamples().catch(console.error);
  }, [createNewProject, audioService]);

  const handleStepToggle = (patternIndex: number, trackIndex: number, stepIndex: number) => {
    if (!project) return;
    
    const currentStep = project.patterns[patternIndex].tracks[trackIndex].steps[stepIndex];
    updateStepData(patternIndex, trackIndex, stepIndex, {
      active: !currentStep.active,
      velocity: currentStep.velocity || 100
    });
  };

  const handleVelocityChange = (patternIndex: number, trackIndex: number, stepIndex: number, velocity: number) => {
    updateStepData(patternIndex, trackIndex, stepIndex, { velocity });
  };

  const handleVolumeChange = (trackIndex: number, volume: number) => {
    updateChannel(trackIndex, 'volume', volume);
    
    // Применяем к аудио сервису
    if (project) {
      audioService.applyChannelParameters(trackIndex, project.patterns[0].tracks[trackIndex].channel);
    }
  };

  if (!project) {
    return (
      <div className="loading">
        <h1>ESP32 Groovebox</h1>
        <p>Loading...</p>
      </div>
    );
  }

  return (
    <div className="app">
      <header className="header">
        <h1>🎵 ESP32 Groovebox</h1>
        <p>Cross-platform Web Interface</p>
      </header>

      <main className="main">
        <div className="controls">
          <div className="project-info">
            <h2>{project.name}</h2>
            <p>BPM: {project.bpm} | Master: {project.masterVolume}%</p>
          </div>
          
          <div className="transport">
            <button onClick={() => audioService.playStep(0)}>
              🎵 Play Kick
            </button>
            <button onClick={() => audioService.playStep(1)}>
              🥁 Play Snare
            </button>
            <button onClick={() => audioService.playStep(2)}>
              🎩 Play HiHat
            </button>
            <button onClick={() => audioService.stopAll()}>
              ⏹️ Stop All
            </button>
          </div>
        </div>

        <div className="sequencer">
          <h2>16-Step Sequencer</h2>
          {project.patterns[0].tracks.map((track, trackIndex) => (
            <div key={track.id} className="track">
              <div className="track-header">
                <h3>{track.name}</h3>
                <div className="volume-control">
                  <label>Volume: {track.channel.volume}%</label>
                  <input
                    type="range"
                    min="0"
                    max="100"
                    value={track.channel.volume}
                    onChange={(e) => handleVolumeChange(trackIndex, parseInt(e.target.value))}
                  />
                </div>
              </div>
              
              <StepGrid
                patternId={project.patterns[0].id}
                trackIndex={trackIndex}
              />
            </div>
          ))}
        </div>

        <div className="info">
          <h3>Zod Validation ✅</h3>
          <p>All data is type-safe and validated</p>
          <p>Tone.js Audio Engine 🎵</p>
          <p>ArduinoJson Compatible 🔄</p>
        </div>
      </main>

      <footer className="footer">
        <p>ESP32 Groovebox - Professional DIY Drum Machine</p>
        <p>Web Interface with Zod + Tone.js + React</p>
      </footer>
    </div>
  );
}

export default App;
