import * as Tone from 'tone';
import { Channel, DSP } from '../../../shared/schemas';

/**
 * Единый аудио движок для ESP32 Groovebox
 * Безопасная работа с Tone.js API
 */
export class GrooveboxAudioEngine {
  private audioContext: AudioContext | null = null;
  private samplers: Tone.Sampler[] = [];
  private masterGain: Tone.Gain;
  private compressor: Tone.Compressor;
  private isInitialized = false;
  private currentBPM = 120;

  constructor() {
    this.masterGain = new Tone.Gain(0.8);
    this.compressor = new Tone.Compressor(-30, 3);
    this.masterGain.chain(this.compressor, Tone.Destination);
  }

  async initialize(): Promise<void> {
    if (this.isInitialized) {
      return;
    }

    try {
      // Запуск аудио контекста
      await Tone.start();
      this.audioContext = Tone.getContext().rawContext as AudioContext;
      
      // Создание сэмплеров
      this.setupSamplers();
      
      this.isInitialized = true;
      console.log('✅ GrooveboxAudioEngine initialized');
    } catch (error) {
      console.error('❌ Failed to initialize GrooveboxAudioEngine:', error);
      throw error;
    }
  }

  dispose(): void {
    if (!this.isInitialized) {
      return;
    }

    try {
      this.stop();
      
      // Очистка сэмплеров
      this.samplers.forEach(sampler => {
        try {
          sampler.dispose();
        } catch (error) {
          console.warn('Error disposing sampler:', error);
        }
      });
      this.samplers = [];

      // Очистка эффектов
      this.masterGain.dispose();
      this.compressor.dispose();

      this.isInitialized = false;
      console.log('✅ GrooveboxAudioEngine disposed');
    } catch (error) {
      console.error('❌ Error during disposal:', error);
    }
  }

  async start(): Promise<void> {
    if (!this.isInitialized) {
      await this.initialize();
    }

    try {
      Tone.Transport.start();
      console.log('▶️ GrooveboxAudioEngine started');
    } catch (error) {
      console.error('❌ Failed to start GrooveboxAudioEngine:', error);
      throw error;
    }
  }

  stop(): void {
    try {
      Tone.Transport.stop();
      Tone.Transport.cancel();
      this.stopAllSamples();
      console.log('⏹️ GrooveboxAudioEngine stopped');
    } catch (error) {
      console.error('❌ Failed to stop GrooveboxAudioEngine:', error);
    }
  }

  pause(): void {
    try {
      Tone.Transport.pause();
      console.log('⏸️ GrooveboxAudioEngine paused');
    } catch (error) {
      console.error('❌ Failed to pause GrooveboxAudioEngine:', error);
    }
  }

  async loadSamples(): Promise<void> {
    if (!this.isInitialized) {
      await this.initialize();
    }

    try {
      const sampleUrls = [
        '/samples/kick.wav',
        '/samples/snare.wav',
        '/samples/hihat_closed.wav',
        '/samples/hihat_open.wav',
        '/samples/tom.wav',
        '/samples/clap.wav',
        '/samples/crash.wav',
        '/samples/perc.wav'
      ];

      const notes = ['C3', 'D3', 'E3', 'F3', 'G3', 'A3', 'B3', 'C4'];

      for (let i = 0; i < 8; i++) {
        const sampler = this.samplers[i];
        if (sampler) {
          // Загрузка сэмпла
          await new Promise<void>((resolve) => {
            const url = sampleUrls[i];
            
            // Создаем буфер и загружаем
            fetch(url)
              .then(response => response.arrayBuffer())
              .then(arrayBuffer => this.audioContext!.decodeAudioData(arrayBuffer))
              .then(audioBuffer => {
                sampler.add(notes[i], audioBuffer);
                console.log(`✅ Sample ${i} (${notes[i]}) loaded`);
                resolve();
              })
              .catch(error => {
                console.warn(`⚠️ Failed to load sample ${i}:`, error);
                resolve(); // Продолжаем даже если сэмпл не загрузился
              });
          });
        }
      }

      console.log('✅ All samples loaded');
    } catch (error) {
      console.error('❌ Failed to load samples:', error);
      throw error;
    }
  }

  playSample(trackIndex: number, velocity: number = 100): void {
    if (!this.isInitialized) {
      console.error('Audio engine not initialized');
      return;
    }

    if (trackIndex < 0 || trackIndex >= 8) {
      console.error(`Invalid track index: ${trackIndex}`);
      return;
    }

    if (velocity < 0 || velocity > 127) {
      console.error(`Invalid velocity: ${velocity}`);
      return;
    }

    try {
      const sampler = this.samplers[trackIndex];
      if (!sampler) {
        console.warn(`Sampler ${trackIndex} not found`);
        return;
      }

      const notes = ['C3', 'D3', 'E3', 'F3', 'G3', 'A3', 'B3', 'C4'];
      const note = notes[trackIndex];
      const normalizedVelocity = velocity / 127;

      sampler.triggerAttackRelease(note, '8n', undefined, normalizedVelocity);
    } catch (error) {
      console.error(`❌ Failed to play sample ${trackIndex}:`, error);
    }
  }

  stopAllSamples(): void {
    try {
      this.samplers.forEach(sampler => {
        try {
          sampler.releaseAll();
        } catch (error) {
          console.warn('Error stopping sampler:', error);
        }
      });
    } catch (error) {
      console.error('❌ Failed to stop all samples:', error);
    }
  }

  applyChannelParameters(trackIndex: number, channel: Channel): void {
    if (!this.isInitialized) {
      console.error('Audio engine not initialized');
      return;
    }

    if (trackIndex < 0 || trackIndex >= 8) {
      console.error(`Invalid track index: ${trackIndex}`);
      return;
    }

    // Валидация параметров
    if (typeof channel.volume !== 'number' || channel.volume < 0 || channel.volume > 100) {
      console.error(`Invalid volume: ${channel.volume}`);
      return;
    }

    if (typeof channel.pitch !== 'number' || channel.pitch < 0 || channel.pitch > 100) {
      console.error(`Invalid pitch: ${channel.pitch}`);
      return;
    }

    try {
      const sampler = this.samplers[trackIndex];
      if (!sampler) {
        console.warn(`Sampler ${trackIndex} not found`);
        return;
      }

      // Volume: 0-100 -> dB
      const volumeDb = channel.volume === 0 ? -60 : 20 * Math.log10(channel.volume / 100);
      sampler.volume.value = volumeDb;

      // Pitch: 0-100 -> transpose (полутона)
      // 50 = 0 полутонов (нормально)
      // 0 = -12 полутонов (ниже)
      // 100 = +12 полутонов (выше)
      const transpose = (channel.pitch - 50) / 50 * 12; // -12..+12 полутонов
      (sampler as any).transpose = transpose;

      console.log(`✅ Applied channel parameters to track ${trackIndex}: volume=${channel.volume}, pitch=${channel.pitch}`);
    } catch (error) {
      console.error(`❌ Failed to apply channel parameters to track ${trackIndex}:`, error);
    }
  }

  applyDSPParameters(dsp: DSP): void {
    if (!this.isInitialized) {
      console.error('Audio engine not initialized');
      return;
    }

    // Валидация DSP параметров
    if (typeof dsp.drive !== 'number' || dsp.drive < 0 || dsp.drive > 100) {
      console.error(`Invalid drive: ${dsp.drive}`);
      return;
    }

    try {
      // Применяем DSP эффекты через Tone.js
      // Note: Это базовая реализация, можно расширить
      
      // Master volume (используем drive как proxy для master volume)
      const masterVolumeDb = dsp.drive === 0 ? -60 : 20 * Math.log10(dsp.drive / 100);
      (this.masterGain as any).volume.value = masterVolumeDb;

      console.log('✅ Applied DSP parameters');
    } catch (error) {
      console.error('❌ Failed to apply DSP parameters:', error);
    }
  }

  setMasterVolume(volume: number): void {
    if (!this.isInitialized) {
      console.error('Audio engine not initialized');
      return;
    }

    if (typeof volume !== 'number' || volume < 0 || volume > 100) {
      console.error(`Invalid volume: ${volume}`);
      return;
    }

    try {
      const volumeDb = volume === 0 ? -60 : 20 * Math.log10(volume / 100);
      (this.masterGain as any).volume.value = volumeDb;
    } catch (error) {
      console.error('❌ Failed to set master volume:', error);
    }
  }

  setBPM(bpm: number): void {
    if (typeof bpm !== 'number' || bpm < 40 || bpm > 300) {
      console.error(`Invalid BPM: ${bpm}`);
      return;
    }

    try {
      this.currentBPM = bpm;
      Tone.Transport.bpm.value = bpm;
    } catch (error) {
      console.error('❌ Failed to set BPM:', error);
    }
  }

  isInitializedEngine(): boolean {
    return this.isInitialized;
  }

  getAudioContext(): AudioContext | null {
    return this.audioContext;
  }

  // Приватные методы

  private setupSamplers(): void {
    const notes = ['C3', 'D3', 'E3', 'F3', 'G3', 'A3', 'B3', 'C4'];

    for (let i = 0; i < 8; i++) {
      try {
        const sampler = new Tone.Sampler({
          urls: { [notes[i]]: '' }, // Пустой URL, установим позже
          baseUrl: '/samples/',
          volume: -6
        }).connect(this.masterGain);

        this.samplers.push(sampler);
      } catch (error) {
        console.error(`❌ Failed to create sampler ${i}:`, error);
        // Добавляем null чтобы сохранить индексацию
        this.samplers.push(null as any);
      }
    }
  }
}

/**
 * Синглтон для аудио движка
 */
let audioEngineInstance: GrooveboxAudioEngine | null = null;

export const getAudioEngine = (): GrooveboxAudioEngine => {
  if (!audioEngineInstance) {
    audioEngineInstance = new GrooveboxAudioEngine();
  }
  return audioEngineInstance;
};

export const disposeAudioEngine = (): void => {
  if (audioEngineInstance) {
    audioEngineInstance.dispose();
    audioEngineInstance = null;
  }
};
