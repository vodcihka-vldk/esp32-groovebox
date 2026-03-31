# Zod Схемы - Руководство по использованию

## 📋 Содержание
- [🎯 Назначение и преимущества](#-назначение-и-преимущества)
- [📦 Установка и настройка](#-установка-и-настройка)
- [🔧 Основные схемы](#-основные-схемы)
- [📖 Практические примеры](#-практические-примеры)
- [🔄 Интеграция с ESP32](#-интеграция-с-esp32)
- [🎵 Tone.js синхронизация](#-tonejs-синхронизация)
- [🧪 Тестирование и отладка](#-тестирование-и-отладка)

---

## 🎯 Назначение и преимущества

### 🔄 **Единый источник истины**
Zod-схемы обеспечивают **типобезопасность** и **валидацию** данных между ESP32 и веб-интерфейсом.

### ✅ **Преимущества:**
- **🔒 Типобезопасность** - Автоматическая проверка типов
- **🛡️ Валидация** - Проверка данных от ESP32
- **🔄 Конвертация** - ArduinoJson ↔ Web формат
- **📝 Автодокументация** - Схемы как документация
- **🧪 Тестирование** - Встроенная валидация
- **🎯 Совместимость** - ArduinoJson 7.4.2

---

## 📦 Установка и настройка

### 📦 **Установка зависимостей:**
```bash
npm install zod tone
npm install -D @types/node
```

### 📁 **Структура файлов:**
```
shared/
├── schemas.ts              # Zod схемы
└── interfaces/
    └── i_sequencer.ts      # Интерфейсы

web/src/
├── hooks/
│   └── useProjectManager.ts # Hook для управления проектами
├── services/
│   └── AudioService.ts      # Tone.js аудио сервис
└── utils/
    └── jsonConverter.ts     # Конвертер JSON
```

---

## 🔧 Основные схемы

### 🎛️ **ChannelSchema - Параметры канала**
```typescript
export const ChannelSchema = z.object({
  volume: z.number().min(0).max(100).default(80),    // Громкость
  pitch: z.number().min(0).max(100).default(50),     // Высота тона
  sample: z.number().min(0).max(8).default(0),       // Индекс сэмпла
  drive: z.number().min(0).max(100).default(0),      // Drive
  start: z.number().min(0).max(100).default(0),      // Начало сэмпла
  length: z.number().min(0).max(100).default(100),    // Длина сэмпла
  attack: z.number().min(0).max(100).default(5),      // Attack
  decay: z.number().min(0).max(100).default(50),      // Decay
});
```

### 🎵 **StepSchema - Структура шага**
```typescript
export const StepSchema = z.object({
  active: z.boolean().default(false),                 // Активен ли шаг
  velocity: z.number().min(0).max(127).optional(),   // Сила удара
  gate: z.number().min(0).max(100).optional(),       // Длина ноты
  pitchShift: z.number().min(-12).max(12).optional(), // Сдвиг высоты
  probability: z.number().min(0).max(100).optional(), // Вероятность
  sampleIndex: z.number().min(0).max(8).optional(),   // Индекс сэмпла
});
```

### 🎛️ **DSPSchema - Эффекты**
```typescript
export const DSPSchema = z.object({
  drive: z.number().min(0).max(100).default(0),       // Drive
  reverb: z.number().min(0).max(100).default(0),      // Reverb
  delay: z.number().min(0).max(100).default(0),       // Delay
  bitcrush: z.number().min(0).max(100).default(0),   // Bitcrush
  eqLow: z.number().min(-24).max(24).default(0),     // EQ Low
  eqMid: z.number().min(-24).max(24).default(0),     // EQ Mid
  eqHigh: z.number().min(-24).max(24).default(0),    // EQ High
});
```

### 📊 **ProjectSchema - Весь проект**
```typescript
export const ProjectSchema = z.object({
  version: z.string().default("1.0"),
  name: z.string().max(32).default("New Beat"),
  bpm: z.number().min(40).max(300).default(132),
  masterVolume: z.number().min(0).max(100).default(80),
  swing: z.number().min(0).max(100).default(50),
  patterns: z.array(PatternSchema).length(8),         // 8 паттернов
  mixer: z.object({
    channels: z.array(MixerChannelSchema).length(8)  // 8 каналов
  }),
  dsp: DSPSchema,                                     // DSP эффекты
});
```

---

## 📖 Практические примеры

### 🔍 **Валидация данных от ESP32**
```typescript
import { validateESP32Data } from '../shared/schemas';

async function loadProjectFromESP32() {
  const response = await fetch('http://esp32.local/project');
  const rawData = await response.json();
  
  try {
    const project = validateESP32Data(rawData);
    setProject(project);
    console.log('✅ Проект успешно загружен и валидирован');
  } catch (error) {
    console.error('❌ Ошибка валидации:', error.message);
    // Показать ошибку пользователю
  }
}
```

### 🔄 **Конвертация форматов**
```typescript
import { webToArduino, arduinoToWeb } from '../shared/schemas';

// Веб → Arduino (для сохранения на ESP32)
function saveToESP32(project: Project) {
  const arduinoFormat = webToArduino(project);
  
  fetch('http://esp32.local/project', {
    method: 'POST',
    body: JSON.stringify(arduinoFormat)
  });
}

// Arduino → Веб (при загрузке)
function loadFromESP32(rawData: unknown) {
  const project = arduinoToWeb(rawData);
  return project;
}
```

### 🎛️ **Безопасное обновление параметров**
```typescript
import { updateChannelParameter, updateStep } from '../shared/schemas';

// Обновление параметра канала с валидацией
function updateChannelVolume(trackIndex: number, volume: number) {
  try {
    const updatedProject = updateChannelParameter(
      project, 
      trackIndex, 
      'volume', 
      volume
    );
    setProject(updatedProject);
  } catch (error) {
    console.error('❌ Неверное значение громкости:', error);
  }
}

// Обновление шага с валидацией
function toggleStep(patternIndex: number, trackIndex: number, stepIndex: number) {
  try {
    const currentStep = project.patterns[patternIndex].tracks[trackIndex].steps[stepIndex];
    const updatedProject = updateStep(
      project, 
      patternIndex, 
      trackIndex, 
      stepIndex, 
      { active: !currentStep.active }
    );
    setProject(updatedProject);
  } catch (error) {
    console.error('❌ Ошибка обновления шага:', error);
  }
}
```

### 🎯 **Создание пустого проекта**
```typescript
import { createEmptyProject } from '../shared/schemas';

function createNewProject(name: string = "New Beat") {
  const project = createEmptyProject(name);
  setProject(project);
  console.log('✅ Создан новый проект:', project.name);
}
```

---

## 🔄 Интеграция с ESP32

### 📤 **Отправка данных на ESP32**
```typescript
// Конвертация веб-формата в ArduinoJson формат
function prepareForESP32(project: Project) {
  const arduinoData = webToArduino(project);
  
  // ArduinoJson ожидает определенный порядок полей
  return {
    name: arduinoData.name,                    // char[16] в ESP32
    bpm: arduinoData.bpm,                      // uint16_t
    masterVolume: arduinoData.masterVolume,    // uint8_t
    patterns: arduinoData.patterns,            // bool[8][8][64]
    channels: arduinoData.channels,            // Channel[8]
    dsp: arduinoData.dsp                       // DSP параметры
  };
}

// Отправка на ESP32
async function saveProjectToESP32(project: Project) {
  const arduinoData = prepareForESP32(project);
  
  const response = await fetch('http://esp32.local/project', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(arduinoData)
  });
  
  if (!response.ok) {
    throw new Error(`ESP32 error: ${response.status}`);
  }
  
  return response.json();
}
```

### 📥 **Получение данных от ESP32**
```typescript
// Ожидаемый формат от ESP32 (ArduinoJson)
interface ESP32Response {
  name: string;           // char[16]
  bpm: number;           // uint16_t
  masterVolume: number;  // uint8_t
  patterns: boolean[][][]; // [8][8][64]
  channels: Array<{     // Channel[8]
    volume: number;      // uint8_t
    pitch: number;       // uint8_t
    sample: number;      // uint8_t
    drive: number;      // uint8_t
    start: number;      // uint8_t
    length: number;     // uint8_t
    attack: number;     // uint8_t
    decay: number;      // uint8_t
  }>;
  dsp: {                // DSP параметры
    drive: number;      // uint8_t
    reverb: number;     // uint8_t
    delay: number;      // uint8_t
    bitcrush: number;   // uint8_t
    eqLow: number;      // uint8_t
    eqMid: number;      // uint8_t
    eqHigh: number;     // uint8_t
  };
}

// Загрузка и конвертация
async function loadProjectFromESP32() {
  const response = await fetch('http://esp32.local/project');
  const esp32Data: ESP32Response = await response.json();
  
  // Конвертация в веб-формат
  const project = arduinoToWeb(esp32Data);
  
  return project;
}
```

---

## 🎵 Tone.js синхронизация

### 🎛️ **Применение параметров канала**
```typescript
import { AudioService } from '../services/AudioService';

const audioService = new AudioService();

// Применение параметров канала с Zod валидацией
function applyChannelToAudio(channelIndex: number, channelData: Channel) {
  // Валидация через Zod
  const validatedChannel = ChannelSchema.parse(channelData);
  
  // Применение к Tone.js
  const sampler = audioService.samplers[channelIndex];
  
  // Громкость (0-100 → 0-1)
  sampler.volume.value = validatedChannel.volume / 100;
  
  // Высота тона (0-100 → -1..+1)
  const pitchShift = (validatedChannel.pitch - 50) / 50;
  sampler.playbackRate = 1 + pitchShift;
  
  // Drive эффект
  const driveAmount = validatedChannel.drive / 100;
  audioService.effects.drive.distortion = driveAmount * 2;
  
  // ADSR огибающая
  sampler.envelope = {
    attack: validatedChannel.attack / 1000,
    decay: validatedChannel.decay / 1000,
    sustain: 0.7,
    release: 0.3
  };
}
```

### 🎛️ **Применение DSP эффектов**
```typescript
function applyDSPToAudio(dspData: DSP) {
  // Валидация через Zod
  const validatedDSP = DSPSchema.parse(dspData);
  
  // Drive
  const driveAmount = validatedDSP.drive / 100;
  audioService.effects.drive.distortion = driveAmount * 2;
  audioService.effects.drive.wet.value = driveAmount;
  
  // Reverb
  audioService.effects.reverb.wet.value = validatedDSP.reverb / 100;
  
  // Delay
  audioService.effects.delay.delayTime.value = (validatedDSP.delay / 100) * 1;
  audioService.effects.delayFeedback.gain.value = (validatedDSP.delay / 100) * 0.8;
  
  // EQ
  audioService.effects.eq.low.gain.value = validatedDSP.eqLow;
  audioService.effects.eq.mid.gain.value = validatedDSP.eqMid;
  audioService.effects.eq.high.gain.value = validatedDSP.eqHigh;
}
```

### 🎵 **Воспроизведение шага**
```typescript
function playStep(channelIndex: number, stepData: Step) {
  // Валидация через Zod
  const validatedStep = StepSchema.parse(stepData);
  
  if (!validatedStep.active) {
    return; // Неактивный шаг
  }
  
  const sampler = audioService.samplers[channelIndex];
  const notes = ['C3', 'D3', 'E3', 'F3', 'G3', 'A3', 'B3', 'C4'];
  const note = notes[channelIndex];
  
  // Velocity (0-127 → 0-1)
  const velocity = (validatedStep.velocity || 100) / 127;
  
  // Воспроизведение
  sampler.triggerAttackRelease(note, '8n', undefined, velocity);
}
```

---

## 🧪 Тестирование и отладка

### ✅ **Валидация тестов**
```typescript
import { ProjectSchema, ChannelSchema } from '../shared/schemas';

// Тест валидных данных
function testValidData() {
  const validProject = {
    name: "Test Beat",
    bpm: 120,
    masterVolume: 80,
    patterns: [], // ... валидные паттерны
    channels: [], // ... валидные каналы
    dsp: { drive: 0, reverb: 20, delay: 30, bitcrush: 0, eqLow: 0, eqMid: 0, eqHigh: 0 }
  };
  
  const result = ProjectSchema.safeParse(validProject);
  console.log('✅ Валидные данные:', result.success);
}

// Тест невалидных данных
function testInvalidData() {
  const invalidProject = {
    name: "Test Beat",
    bpm: 500, // ❌ Выше максимума (300)
    masterVolume: 150, // ❌ Выше максимума (100)
    patterns: [], // ... валидные паттерны
    channels: [], // ... валидные каналы
    dsp: { drive: 150, reverb: 20, delay: 30, bitcrush: 0, eqLow: 0, eqMid: 0, eqHigh: 0 }
  };
  
  const result = ProjectSchema.safeParse(invalidProject);
  if (!result.success) {
    console.log('❌ Невалидные данные:', result.error.format());
  }
}
```

### 🔍 **Отладка конвертации**
```typescript
function debugConversion(project: Project) {
  console.group('🔍 Debug Conversion');
  
  // Исходные данные
  console.log('📥 Original project:', project);
  
  // Конвертация в Arduino формат
  const arduinoData = webToArduino(project);
  console.log('🔄 Arduino format:', arduinoData);
  
  // Обратная конвертация
  const webData = arduinoToWeb(arduinoData);
  console.log('🔄 Web format:', webData);
  
  // Сравнение
  const isEqual = JSON.stringify(project) === JSON.stringify(webData);
  console.log('✅ Round-trip successful:', isEqual);
  
  console.groupEnd();
}
```

### 📊 **Статистика валидации**
```typescript
function validateProjectStats(project: unknown) {
  const result = ProjectSchema.safeParse(project);
  
  if (result.success) {
    const stats = {
      name: result.data.name.length,
      bpm: result.data.bpm,
      patterns: result.data.patterns.length,
      tracks: result.data.patterns[0]?.tracks.length || 0,
      steps: result.data.patterns[0]?.tracks[0]?.steps.length || 0,
      effects: Object.keys(result.data.dsp).length
    };
    
    console.log('📊 Project stats:', stats);
    return stats;
  } else {
    console.error('❌ Validation failed:', result.error.format());
    return null;
  }
}
```

---

## 🎯 **Лучшие практики**

### ✅ **Рекомендации:**
1. **Всегда валидируйте** данные от ESP32 через `safeParse()`
2. **Используйте утилиты** `webToArduino()` и `arduinoToWeb()`
3. **Обрабатывайте ошибки** валидации gracefully
4. **Логируйте конвертации** для отладки
5. **Тестируйте граничные случаи** (min/max значения)

### 🚫 **Избегайте:**
1. **Прямых мутаций** без валидации
2. **Игнорирования ошибок** Zod
3. **Жестко закодированных** конвертаций
4. **Пропуска валидации** в продакшене

---

## 🔄 **Рабочий процесс**

1. **📝 Разработка** - Используйте Zod схемы как контракты
2. **🧪 Тестирование** - Валидируйте все данные
3. **🔄 Конвертация** - Используйте встроенные утилиты
4. **🎵 Применение** - Синхронизируйте с Tone.js
5. **📤 Отправка** - Конвертируйте в ArduinoJson формат
6. **📥 Получение** - Валидируйте и конвертируйте обратно

Этот подход обеспечивает **надежность**, **типобезопасность** и **совместимость** между ESP32 и веб-интерфейсом!
