import React from 'react';
import {
  useProject,
  useIsPlaying,
  useBPM,
  useCurrentPattern,
  useCurrentTrack,
  useCurrentPatternData,
  useGrooveboxActions,
  useProjectUtils,
} from './store/useGrooveboxStore';
import './App.css';

const TRACK_NAMES = ['Kick', 'Snare', 'Clap', 'HiHat C', 'HiHat O', 'Crash', 'Tom', 'Perc'];

function StepRow({
  trackIndex,
  trackName,
  steps,
  isSelected,
  volume,
  onToggle,
  onSelect,
  onVolumeChange,
}: {
  trackIndex: number;
  trackName: string;
  steps: { active: boolean; velocity?: number }[];
  isSelected: boolean;
  volume: number;
  onToggle: (step: number) => void;
  onSelect: () => void;
  onVolumeChange: (v: number) => void;
}) {
  return (
    <div className={`track-row ${isSelected ? 'selected' : ''}`} onClick={onSelect}>
      <div className="track-label">
        <span className="track-name">{trackName}</span>
        <input
          type="range"
          min={0}
          max={100}
          value={volume}
          className="vol-slider"
          onClick={(e) => e.stopPropagation()}
          onChange={(e) => onVolumeChange(Number(e.target.value))}
        />
      </div>
      <div className="steps">
        {steps.slice(0, 16).map((s, i) => (
          <button
            key={i}
            className={`step ${s.active ? 'on' : ''} ${i % 4 === 0 ? 'bar' : ''}`}
            onClick={(e) => {
              e.stopPropagation();
              onToggle(i);
            }}
          />
        ))}
      </div>
    </div>
  );
}

function App() {
  const project = useProject();
  const isPlaying = useIsPlaying();
  const bpm = useBPM();
  const currentPattern = useCurrentPattern();
  const currentTrack = useCurrentTrack();
  const patternData = useCurrentPatternData();

  const {
    setPlaying,
    setBPM,
    toggleStep,
    updateChannel,
    setCurrentTrack,
    setCurrentPattern,
  } = useGrooveboxActions();
  const { createNewProject, exportToESP32 } = useProjectUtils();

  // Init project
  React.useEffect(() => {
    createNewProject('ESP32 Groovebox Demo');
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  if (!project || !patternData) {
    return (
      <div className="app loading-screen">
        <h1>🎵 ESP32 Groovebox</h1>
        <p>Loading...</p>
      </div>
    );
  }

  return (
    <div className="app">
      {/* Header */}
      <header className="header">
        <h1>🎵 ESP32 Groovebox</h1>
        <span className="subtitle">Web UI · Zod + Zustand + React</span>
      </header>

      {/* Transport */}
      <section className="transport">
        <button className={`btn ${isPlaying ? 'active' : ''}`} onClick={() => setPlaying(!isPlaying)}>
          {isPlaying ? '⏸ Pause' : '▶ Play'}
        </button>
        <button className="btn" onClick={() => setPlaying(false)}>⏹ Stop</button>
        <button className="btn" onClick={() => {
          const data = exportToESP32();
          if (data) {
            console.log('ESP32 JSON:', data);
            alert('Exported — check console');
          }
        }}>📤 Export</button>

        <div className="bpm-block">
          <label>BPM</label>
          <input
            type="number"
            min={40}
            max={300}
            value={bpm}
            onChange={(e) => setBPM(Number(e.target.value))}
          />
          <input
            type="range"
            min={40}
            max={300}
            value={bpm}
            onChange={(e) => setBPM(Number(e.target.value))}
          />
        </div>
      </section>

      {/* Pattern selector */}
      <section className="patterns">
        {Array.from({ length: 8 }, (_, i) => (
          <button
            key={i}
            className={`pat-btn ${currentPattern === i ? 'active' : ''}`}
            onClick={() => setCurrentPattern(i)}
          >
            P{i + 1}
          </button>
        ))}
      </section>

      {/* Sequencer grid */}
      <section className="sequencer">
        {patternData.tracks.map((track, ti) => (
          <StepRow
            key={track.id}
            trackIndex={ti}
            trackName={TRACK_NAMES[ti] || track.name}
            steps={track.steps}
            isSelected={currentTrack === ti}
            volume={track.channel.volume}
            onToggle={(si) => toggleStep(currentPattern, ti, si)}
            onSelect={() => setCurrentTrack(ti)}
            onVolumeChange={(v) => updateChannel(ti, { volume: v })}
          />
        ))}
      </section>

      {/* DSP */}
      <section className="dsp">
        <h3>DSP Effects</h3>
        <div className="dsp-grid">
          {(['drive', 'reverb', 'delay', 'bitcrush'] as const).map((fx) => (
            <div key={fx} className="dsp-knob">
              <label>{fx}: {(project.dsp as any)[fx]}%</label>
              <input
                type="range"
                min={0}
                max={100}
                value={(project.dsp as any)[fx]}
              />
            </div>
          ))}
        </div>
      </section>

      <footer className="footer">
        ESP32 Groovebox — Cross-platform Drum Machine
      </footer>
    </div>
  );
}

export default App;
