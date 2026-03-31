export interface ISequencer {
  // Управление паттернами
  createPattern(name: string): Pattern;
  getPattern(id: string): Pattern | null;
  deletePattern(id: string): boolean;
  
  // Управление шагами
  setStep(patternId: string, track: number, step: number, data: StepData): void;
  getStep(patternId: string, track: number, step: number): StepData;
  clearStep(patternId: string, track: number, step: number): void;
  
  // Воспроизведение
  play(): void;
  pause(): void;
  stop(): void;
  setBPM(bpm: number): void;
  getBPM(): number;
  
  // Parameter locks
  setParameterLock(patternId: string, track: number, step: number, param: string, value: number): void;
  getParameterLock(patternId: string, track: number, step: number, param: string): number | null;
  clearParameterLocks(patternId: string, track: number): void;
  
  // События
  onStep(callback: (data: StepEventData) => void): () => void;
  onPatternChange(callback: (pattern: Pattern) => void): () => void;
  onBPMChange(callback: (bpm: number) => void): () => void;
}

export interface Pattern {
  id: string;
  name: string;
  bpm: number;
  length: number;
  tracks: Track[];
  parameterLocks: ParameterLock[];
}

export interface Track {
  id: string;
  name: string;
  sampleId: string;
  muted: boolean;
  solo: boolean;
  volume: number;
  pan: number;
  steps: Step[];
}

export interface Step {
  active: boolean;
  velocity: number;
  note: number;
  length: number;
  offset: number;
}

export interface StepData {
  active?: boolean;
  velocity?: number;
  note?: number;
  length?: number;
  offset?: number;
}

export interface ParameterLock {
  track: number;
  step: number;
  parameter: string;
  value: number;
  type: LockType;
}

export interface StepEventData {
  patternId: string;
  step: number;
  track: number;
  timestamp: number;
}

export enum LockType {
  BASIC = 'basic',
  SLIDE = 'slide',
  RANDOM = 'random',
  PROBABILITY = 'probability',
  CONDITIONAL = 'conditional'
}
