# ESP32 Groovebox

Аппаратный грувбокс / драм-машина на базе **ESP32** с PSRAM.  
Кросс-платформенный проект: прошивка (C++ / PlatformIO) + веб-интерфейс (React / TypeScript / Vite).

## Возможности

- **Сэмплер** — 7 ударных инструментов (kick, snare, clap, hihat, crash, tom)
- **16-шаговый секвенсор** с FreeRTOS-задачей
- **DSP-эффекты** — drive, reverb, delay, bitcrush, EQ, компрессор, лимитер, сатурация
- **I2S-аудиовыход** (PCM5102A / MAX98357A)
- **LCD 1602 I2C** дисплей + энкодеры для навигации
- **Хранение проектов** на SD-карте (JSON)
- **Веб-интерфейс** — React + Web Audio API для предпросмотра и редактирования паттернов

## Структура проекта

```
├── src/                  # Прошивка ESP32 (C++ / Arduino)
│   ├── main.cpp          # Точка входа, FreeRTOS задачи
│   ├── audio/            # Сэмплер, DSP-эффекты
│   ├── sequencer/        # Секвенсор
│   ├── ui/               # LCD-дисплей, энкодеры
│   ├── storage/          # Сохранение проектов (SD)
│   ├── config/           # Конфигурация пинов
│   └── state/            # Глобальное состояние
├── web/                  # Веб-интерфейс (React + Vite)
├── hardware/             # KiCad: схемы, PCB, BOM, корпус
│   ├── schematics/
│   ├── pcb/
│   ├── bom/
│   └── mechanical/
├── shared/               # Общие типы/схемы (TS)
├── tools/                # Утилиты (wav2c, генерация сэмплов)
├── samples/              # WAV-файлы сэмплов
├── docs/                 # Документация
└── platformio.ini        # Конфигурация PlatformIO
```

## Быстрый старт

### Прошивка (PlatformIO)

```bash
# Сборка
pio run

# Прошивка
pio run --target upload

# Монитор
pio device monitor
```

### Веб-интерфейс

```bash
cd web
npm install
npm run dev
```

## Аппаратная часть

| Компонент         | Модель               |
|-------------------|----------------------|
| MCU               | ESP32 DevKit V1      |
| Аудио DAC         | PCM5102A (I2S)       |
| Дисплей           | LCD 1602 (I2C)       |
| Хранилище         | microSD (SPI)        |
| Управление        | Rotary encoders      |

Подробности: [`hardware/README.md`](hardware/README.md)

## Лицензия

MIT
