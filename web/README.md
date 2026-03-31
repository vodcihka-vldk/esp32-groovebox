# ESP32 Groovebox Web Interface

Профессиональный веб-интерфейс для ESP32 Groovebox с Zod валидацией, Tone.js аудио и Zustand управлением состоянием.

## 🚀 Быстрый запуск

### Установка зависимостей
```bash
npm install
```

### Запуск дев-сервера
```bash
npm run dev
```

### Сборка для продакшена
```bash
npm run build
```

### Предпросмотр сборки
```bash
npm run preview
```

## 📋 Архитектура

### 🔧 Технологический стек
- **React 18** - UI фреймворк
- **TypeScript** - Типобезопасность
- **Vite** - Сборщик и дев-сервер
- **Zod** - Валидация данных
- **Tone.js** - Web Audio API
- **Zustand** - Управление состоянием
- **Immer** - Имутабельные обновления

### 📁 Структура проекта
```
web/
├── src/
│   ├── components/          # React компоненты
│   │   └── sequencer/
│   │       └── StepGrid.tsx
│   ├── engine/              # Аудио движок
│   │   ├── bridge.ts        # Zod ↔ Tone.js мост
│   │   └── sequencer.ts     # 16-шаговый секвенсор
│   ├── store/               # Zustand store
│   │   └── useGrooveboxStore.ts
│   ├── hooks/               # React hooks
│   │   └── useSequencer.ts
│   ├── utils/               # Утилиты
│   │   └── jsonConverter.ts
│   ├── App.tsx              # Основное приложение
│   └── main.tsx             # Точка входа
├── public/                  # Статические файлы
├── package.json
├── vite.config.ts
├── tsconfig.json
└── README.md
```

## 🎵 Функциональность

### 🎛️ Секвенсор
- **64 шага** (4 бара по 16 шагов)
- **8 треков** одновременно
- **16-voice полифония**
- **Точный тайминг** через Tone.Sequence
- **Parameter locks** для каждого шага

### 🎛️ Управление параметрами
- **Громкость** (0-100%)
- **Высота тона** (0.5x-2x)
- **ADSR огибающая** (Attack/Decay)
- **DSP эффекты** (Drive, Reverb, Delay, Bitcrush)
- **3-band EQ** (Low/Mid/High)

### 🔄 ESP32 Интеграция
- **ArduinoJson 7.4.2** совместимость
- **Zod валидация** данных
- **Двусторонняя синхронизация**
- **JSON конвертация** форматов

## 📊 Zod Схемы

### 🎯 Основные типы
```typescript
// Параметры канала
interface Channel {
  volume: number;    // 0-100
  pitch: number;     // 0-100 (50=нормально)
  sample: number;    // 0-8
  drive: number;     // 0-100
  start: number;     // 0-100%
  length: number;    // 0-100%
  attack: number;    // 0-100ms
  decay: number;     // 0-100ms
}

// Шаг паттерна
interface Step {
  active: boolean;           // Активен ли шаг
  velocity?: number;        // 0-127 (сила удара)
  gate?: number;            // 0-100% (длина ноты)
  pitchShift?: number;      // -12..+12 полутонов
  probability?: number;     // 0-100%
  sampleIndex?: number;     // 0-8
}

// Весь проект
interface Project {
  version: string;
  name: string;
  bpm: number;              // 40-300
  masterVolume: number;     // 0-100
  patterns: Pattern[8];     // 8 паттернов
  mixer: MixerChannel[8];   // 8 каналов микшера
  dsp: DSP;                 // Эффекты
}
```

### 🔄 Конвертация форматов
```typescript
// Веб → ArduinoJson
const arduinoData = webToArduino(project);

// ArduinoJson → Веб
const webData = arduinoToWeb(arduinoJson);

// Валидация
const validated = ProjectSchema.parse(data);
```

## 🎵 Tone.js Интеграция

### 🎛️ Аудио цепь
```
Input → Drive → EQ Low → EQ Mid → EQ High → Delay → Reverb → Bitcrush → Master → Compressor → Output
```

### 🎯 Маппинг параметров
```typescript
// Volume: 0-100 → -60dB до 0dB
player.volume.value = Tone.gainToDb(channel.volume / 100);

// Pitch: 50 → 1.0x, 0 → 0.5x, 100 → 2.0x
player.playbackRate = channel.pitch / 50;

// ADSR: ms → seconds
player.fadeIn = channel.attack / 1000;
player.fadeOut = channel.decay / 1000;
```

### 🎹 Сэмплеры
- **8 сэмплеров** для каждого трека
- **MIDI ноты**: C3, D3, E3, F3, G3, A3, B3, C4
- **Velocity**: 0-127 с нормализацией
- **Предзагрузка** сэмплов

## 🔄 Zustand Store

### 📊 Оптимизированное состояние
```typescript
// Selectors для оптимизации ререндеров
const project = useProject();
const isPlaying = useIsPlaying();
const currentPattern = useCurrentPattern();

// Actions с Immer
const { toggleStep, updateChannel, setBPM } = useGrooveboxActions();
```

### 🎯 Эффективные обновления
- **Immer** для имутабельных обновлений
- **Selective selectors** для минимизации ререндеров
- **Batch updates** для производительности

## 🎛️ Компоненты

### 📐 StepGrid
- **16x64 сетка** шагов
- **Интерактивные кнопки** с velocity
- **Visual feedback** для текущего шага
- **Touch-friendly** для мобильных устройств

### 🎛️ Transport Controls
- **Play/Pause/Stop** управление
- **BPM регулировка** (40-300)
- **Pattern selection** (1-8)
- **Track selection** (1-8)

### 🎵 DSP Controls
- **Drive** дисторшн
- **Reverb** реверберация
- **Delay** дилэй с фидбэком
- **Bitcrush** биткрашер
- **3-band EQ** эквалайзер

## 🔄 ESP32 Коммуникация

### 📤 Экспорт в ESP32
```typescript
const esp32Data = {
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

fetch('http://esp32.local/project', {
  method: 'POST',
  body: JSON.stringify(esp32Data)
});
```

### 📥 Импорт из ESP32
```typescript
const response = await fetch('http://esp32.local/project');
const esp32Data = await response.json();

// Конвертация в веб-формат
const project = arduinoToWeb(esp32Data);
setProject(project);
```

## 🎛️ Разработка

### 🧪 Тестирование
```bash
# Типизация
npm run type-check

# Линтинг
npm run lint
npm run lint:fix
```

### 📦 Сборка
```bash
# Дев-сборка
npm run dev

# Продакшен сборка
npm run build

# Предпросмотр
npm run preview
```

## 🎯 Особенности

### ✅ Преимущества
- **🔒 Типобезопасность** через Zod
- **🎵 Профессиональный звук** через Tone.js
- **⚡ Оптимизированное состояние** через Zustand
- **🔄 ESP32 совместимость** с ArduinoJson
- **📱 Responsive дизайн** для всех устройств
- **🎛️ Полный функционал** секвенсора

### 🎯 Целевая аудитория
- **DIY музыканты** и продюсеры
- **Embedded разработчики** ESP32
- **Web Audio энтузиасты**
- **Электронные музыканты**

## 📝 Лицензия

MIT License - свободное использование и модификация

## 🤝 Вклад

Welcome contributions! Please read the contributing guidelines and submit pull requests.

---

**ESP32 Groovebox Web Interface** - Professional cross-platform drum machine interface
