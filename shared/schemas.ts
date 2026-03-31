import { z } from 'zod';

/**
 * ESP32 Groovebox - Shared Zod Schemas
 * Единый источник истины для данных между ESP32 и веб-интерфейсом
 * Совместимо с ArduinoJson 7.4.2 структурой
 */

// ============================================================================
// Базовые типы данных
// ============================================================================

/** MIDI нота (36-96) */
export const NoteSchema = z.number().min(36).max(96).default(60);

/** Velocity (0-127) */
export const VelocitySchema = z.number().min(0).max(127).default(100);

/** Процентное значение (0-100) */
export const PercentSchema = z.number().min(0).max(100);

/** Громкость в децибелах (-24 до +24) */
export const DecibelSchema = z.number().min(-24).max(24).default(0);

/** BPM (40-300) */
export const BPMSchema = z.number().min(40).max(300).default(132);

// ============================================================================
// 1. Параметры канала (аналог struct Channel в C++)
// ============================================================================

export const ChannelSchema = z.object({
  /** Громкость канала (0-100, 80 по умолчанию) */
  volume: PercentSchema.default(80),
  
  /** Высота тона (0-100, 50=нормально) */
  pitch: PercentSchema.default(50),
  
  /** Индекс сэмпла (0-8) */
  sample: z.number().min(0).max(8).default(0),
  
  /** Drive/distortion (0-100) */
  drive: PercentSchema.default(0),
  
  /** Начало сэмпла в % (0-100) */
  start: PercentSchema.default(0),
  
  /** Длина сэмпла в % (0-100) */
  length: PercentSchema.default(100),
  
  /** Attack время в ms (0-100) */
  attack: PercentSchema.default(5),
  
  /** Decay время в ms (0-100) */
  decay: PercentSchema.default(50),
});

// ============================================================================
// 2. Структура шага (Step)
// ============================================================================

export const StepSchema = z.object({
  /** Активен ли шаг (true/false) */
  active: z.boolean().default(false),
  
  /** Сила удара (0-127) - для будущего расширения */
  velocity: VelocitySchema.optional(),
  
  /** Длина ноты в % (0-100) - для будущего расширения */
  gate: PercentSchema.optional(),
  
  /** Сдвиг высоты тона в полутонах (-12..+12) - для будущего расширения */
  pitchShift: z.number().min(-12).max(12).optional(),
  
  /** Вероятность активации (0-100) - для будущего расширения */
  probability: PercentSchema.optional(),
  
  /** Индекс сэмпла для шага - для будущего расширения */
  sampleIndex: z.number().min(0).max(8).optional(),
});

// ============================================================================
// 3. Parameter Locks
// ============================================================================

export const ParameterLockTypeSchema = z.enum(['basic', 'slide', 'random', 'probability', 'conditional']);

export const ParameterLockSchema = z.object({
  /** Номер трека (0-7) */
  track: z.number().min(0).max(7),
  
  /** Номер шага (0-63) */
  step: z.number().min(0).max(63),
  
  /** Параметр для блокировки */
  parameter: z.enum(['volume', 'pitch', 'drive', 'filter', 'pan', 'effect']),
  
  /** Значение параметра */
  value: z.number(),
  
  /** Тип блокировки */
  type: ParameterLockTypeSchema.default('basic'),
});

// ============================================================================
// 4. Эффекты (DSP)
// ============================================================================

export const DSPSchema = z.object({
  /** Drive/distortion (0-100) */
  drive: PercentSchema.default(0),
  
  /** Reverb (0-100) */
  reverb: PercentSchema.default(0),
  
  /** Delay (0-100) */
  delay: PercentSchema.default(0),
  
  /** Bitcrush (0-100) */
  bitcrush: PercentSchema.default(0),
  
  /** EQ High (-24..+24 dB) */
  eqHigh: DecibelSchema,
  
  /** EQ Mid (-24..+24 dB) */
  eqMid: DecibelSchema,
  
  /** EQ Low (-24..+24 dB) */
  eqLow: DecibelSchema,
});

// ============================================================================
// 5. Микшер параметры
// ============================================================================

export const MixerChannelSchema = z.object({
  /** ID канала */
  id: z.string(),
  
  /** Громкость (0-100) */
  volume: PercentSchema.default(80),
  
  /** Панорама (-50..+50, 0=center) */
  pan: z.number().min(-50).max(50).default(0),
  
  /** Mute включен */
  mute: z.boolean().default(false),
  
  /** Solo включен */
  solo: z.boolean().default(false),
});

// ============================================================================
// 6. Паттерн
// ============================================================================

export const PatternSchema = z.object({
  /** ID паттерна */
  id: z.string().default("pattern_0"),
  
  /** Название паттерна */
  name: z.string().max(32).default("New Pattern"),
  
  /** Длина паттерна в шагах (16, 32, 64) */
  length: z.enum(['16', '32', '64']).default('64'),
  
  /** Swing в % (0-100) */
  swing: PercentSchema.default(50),
  
  /** Треки паттерна (8 треков) */
  tracks: z.array(z.object({
    /** ID трека */
    id: z.string(),
    
    /** Название трека */
    name: z.string().max(32),
    
    /** ID сэмпла */
    sampleId: z.string(),
    
    /** Параметры канала */
    channel: ChannelSchema,
    
    /** Шаги трека (64 шага) */
    steps: z.array(StepSchema).length(64),
    
    /** Parameter locks для трека */
    parameterLocks: z.array(ParameterLockSchema).optional(),
  })).length(8),
});

// ============================================================================
// 7. Весь проект (Root Object)
// ============================================================================

export const ProjectSchema = z.object({
  /** Версия формата */
  version: z.string().default("1.0"),
  
  /** Название проекта */
  name: z.string().max(32).default("New Beat"),
  
  /** BPM (40-300) */
  bpm: BPMSchema,
  
  /** Master громкость (0-100) */
  masterVolume: PercentSchema.default(80),
  
  /** Swing глобальный (0-100) */
  swing: PercentSchema.default(50),
  
  /** Паттерны проекта (8 паттернов) */
  patterns: z.array(PatternSchema).length(8),
  
  /** Каналы микшера (8 каналов) */
  mixer: z.object({
    channels: z.array(MixerChannelSchema).length(8)
  }),
  
  /** Глобальные DSP эффекты */
  dsp: DSPSchema,
  
  /** Метаданные проекта */
  metadata: z.object({
    /** Время создания */
    createdAt: z.string().optional(),
    
    /** Время последнего изменения */
    updatedAt: z.string().optional(),
    
    /** Автор проекта */
    author: z.string().optional(),
    
    /** Комментарии */
    comments: z.string().optional(),
  }).optional(),
});

// ============================================================================
// 8. ArduinoJson совместимая структура (для ESP32)
// ============================================================================

/** Упрощенная структура для ArduinoJson - сохраняет порядок полей */
export const ArduinoProjectSchema = z.object({
  /** Название проекта */
  name: z.string().max(16), // Ограничение ESP32
  
  /** BPM */
  bpm: BPMSchema,
  
  /** Master громкость */
  masterVolume: PercentSchema,
  
  /** Паттерны в формате boolean [8][8][64] */
  patterns: z.array(z.array(z.array(z.boolean()))).length(8).length(8).length(64),
  
  /** Параметры каналов */
  channels: z.array(ChannelSchema).length(8),
  
  /** DSP параметры */
  dsp: DSPSchema,
});

// ============================================================================
// 9. События и состояния
// ============================================================================

export const StepEventSchema = z.object({
  /** ID паттерна */
  patternId: z.string(),
  
  /** Номер трека */
  track: z.number().min(0).max(7),
  
  /** Номер шага */
  step: z.number().min(0).max(63),
  
  /** Время события */
  timestamp: z.number(),
  
  /** Данные шага */
  stepData: StepSchema,
});

export const SequencerStateSchema = z.object({
  /** Секвенсор играет */
  isPlaying: z.boolean(),
  
  /** Текущий паттерн */
  currentPattern: z.number().min(0).max(7),
  
  /** Текущий шаг */
  currentStep: z.number().min(0).max(63),
  
  /** Выбранный канал */
  selectedChannel: z.number().min(0).max(7),
});

// ============================================================================
// Автоматическая генерация типов из схем
// ============================================================================

export type Project = z.infer<typeof ProjectSchema>;
export type Pattern = z.infer<typeof PatternSchema>;
export type Channel = z.infer<typeof ChannelSchema>;
export type Step = z.infer<typeof StepSchema>;
export type DSP = z.infer<typeof DSPSchema>;
export type MixerChannel = z.infer<typeof MixerChannelSchema>;
export type ParameterLock = z.infer<typeof ParameterLockSchema>;
export type ArduinoProject = z.infer<typeof ArduinoProjectSchema>;
export type StepEvent = z.infer<typeof StepEventSchema>;
export type SequencerState = z.infer<typeof SequencerStateSchema>;

// ============================================================================
// Утилиты для работы со схемами
// ============================================================================

/**
 * Валидация данных от ESP32
 */
export function validateESP32Data(data: unknown): Project {
  const result = ProjectSchema.safeParse(data);
  
  if (!result.success) {
    console.error("❌ Данные с ESP32 не соответствуют схеме:", result.error.format());
    throw new Error(`Invalid ESP32 data: ${result.error.message}`);
  }
  
  return result.data;
}

/**
 * Конвертация веб-формата в ArduinoJson формат
 */
export function webToArduino(project: Project): ArduinoProject {
  return {
    name: project.name.substring(0, 16), // Ограничение ESP32
    bpm: project.bpm,
    masterVolume: project.masterVolume,
    patterns: project.patterns.map(pattern =>
      pattern.tracks.map(track =>
        track.steps.map(step => step.active)
      )
    ),
    channels: project.patterns[0].tracks.map(track => track.channel), // Первый паттерн
    dsp: project.dsp
  };
}

/**
 * Конвертация ArduinoJson в веб-формат
 */
export function arduinoToWeb(arduinoProject: ArduinoProject): Project {
  return {
    version: "1.0",
    name: arduinoProject.name,
    bpm: arduinoProject.bpm,
    masterVolume: arduinoProject.masterVolume,
    swing: 50,
    patterns: arduinoProject.patterns.map((pattern, pIndex) => ({
      id: `pattern_${pIndex}`,
      name: `Pattern ${pIndex + 1}`,
      length: '64',
      swing: 50,
      tracks: pattern.map((channel, cIndex) => ({
        id: `track_${cIndex}`,
        name: `Track ${cIndex + 1}`,
        sampleId: `sample_${arduinoProject.channels[cIndex].sample}`,
        channel: arduinoProject.channels[cIndex],
        steps: Array.from({ length: 64 }, (_, sIndex) => ({
          active: channel[sIndex]
        })),
        parameterLocks: []
      }))
    })),
    mixer: {
      channels: arduinoProject.channels.map((channel, index) => ({
        id: `track_${index}`,
        volume: channel.volume,
        pan: 0,
        mute: false,
        solo: false
      }))
    },
    dsp: arduinoProject.dsp,
    metadata: {
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString()
    }
  };
}

/**
 * Создание пустого проекта
 */
export function createEmptyProject(name: string = "New Beat"): Project {
  return ProjectSchema.parse({
    name,
    bpm: 132,
    masterVolume: 80,
    swing: 50,
    patterns: Array.from({ length: 8 }, (_, pIndex) => ({
      id: `pattern_${pIndex}`,
      name: `Pattern ${pIndex + 1}`,
      length: '64',
      swing: 50,
      tracks: Array.from({ length: 8 }, (_, tIndex) => ({
        id: `track_${tIndex}`,
        name: `Track ${tIndex + 1}`,
        sampleId: `sample_${tIndex}`,
        channel: {
          volume: 80,
          pitch: 50,
          sample: tIndex,
          drive: 0,
          start: 0,
          length: 100,
          attack: 5,
          decay: 50
        },
        steps: Array.from({ length: 64 }, () => ({ active: false })),
        parameterLocks: []
      }))
    })),
    mixer: {
      channels: Array.from({ length: 8 }, (_, index) => ({
        id: `track_${index}`,
        volume: 80,
        pan: 0,
        mute: false,
        solo: false
      }))
    },
    dsp: {
      drive: 0,
      reverb: 0,
      delay: 0,
      bitcrush: 0,
      eqHigh: 0,
      eqMid: 0,
      eqLow: 0
    },
    metadata: {
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString()
    }
  });
}

/**
 * Валидация и безопасное обновление параметра канала
 */
export function updateChannelParameter(
  project: Project,
  trackIndex: number,
  parameter: keyof Channel,
  value: number
): Project {
  const updatedProject = { ...project };
  const pattern = updatedProject.patterns[0]; // Обновляем первый паттерн
  const track = pattern.tracks[trackIndex];
  
  // Валидация через схему
  const updatedChannel = ChannelSchema.parse({
    ...track.channel,
    [parameter]: value
  });
  
  track.channel = updatedChannel;
  return updatedProject;
}

/**
 * Валидация и безопасное обновление шага
 */
export function updateStep(
  project: Project,
  patternIndex: number,
  trackIndex: number,
  stepIndex: number,
  stepData: Partial<Step>
): Project {
  const updatedProject = { ...project };
  const pattern = updatedProject.patterns[patternIndex];
  const track = pattern.tracks[trackIndex];
  
  // Валидация через схему
  const updatedStep = StepSchema.parse({
    ...track.steps[stepIndex],
    ...stepData
  });
  
  track.steps[stepIndex] = updatedStep;
  return updatedProject;
}

