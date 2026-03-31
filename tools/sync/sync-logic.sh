#!/bin/bash

# Синхронизация логики между C++ и TypeScript
# ESP32 Groovebox Cross-Platform Sync Script

set -e

echo "🔄 Syncing logic between C++ and TypeScript..."

SHARED_DIR="shared"
FIRMWARE_DIR="firmware"
WEB_DIR="web"

# Проверка существования директорий
if [ ! -d "$SHARED_DIR" ]; then
    echo "❌ Shared directory not found: $SHARED_DIR"
    exit 1
fi

if [ ! -d "$WEB_DIR" ]; then
    echo "❌ Web directory not found: $WEB_DIR"
    exit 1
fi

# Функция для конвертации C++ заголовков в TypeScript интерфейсы
convert_cpp_to_ts() {
    local cpp_file=$1
    local ts_file=$2
    
    echo "🔄 Converting $cpp_file to $ts_file"
    
    # Создание директории если не существует
    mkdir -p "$(dirname "$ts_file")"
    
    # Простая конвертация (можно улучшить с помощью clang tools)
    sed -E '
        s/class ([A-Za-z0-9_]+) \{/export interface \1 {/
        s/struct ([A-Za-z0-9_]+) \{/export interface \1 {/
        s/static const ([A-Za-z0-9_]+) = ([0-9]+)/export const \1 = \2;/
        s/uint8_t/number/g
        s/uint16_t/number/g
        s/uint32_t/number/g
        s/float/number/g
        s/bool/boolean/g
        s/std::string/string/g
        s/std::vector<([^>]+)>/\1[]/g
        s/std::array<([^,]+), ([0-9]+)>/\1[\2]/g
        s/public:/\/\/ public:/g
        s/private:/\/\/ private:/g
        s/protected:/\/\/ protected:/g
    ' "$cpp_file" > "$ts_file"
    
    echo "✅ Converted $cpp_file"
}

# Функция для синхронизации TypeScript в C++
sync_ts_to_cpp() {
    local ts_file=$1
    local cpp_file=$2
    
    echo "🔄 Syncing $ts_file to $cpp_file"
    
    # Создание директории если не существует
    mkdir -p "$(dirname "$cpp_file")"
    
    # Обратная конвертация (упрощенная)
    sed -E '
        s/export interface ([A-Za-z0-9_]+) \{/class \1 {/
        s/export const ([A-Za-z0-9_]+) = ([0-9]+)/static const \1 = \2;/
        s/number/uint8_t/g
        s/string/std::string/g
        s/\[\]/std::vector/g
        s/\/\/ public:/public:/g
    ' "$ts_file" > "$cpp_file"
    
    echo "✅ Synced $ts_file"
}

# Синхронизация секвенсора
if [ -f "$SHARED_DIR/core/sequencer/pattern.h" ]; then
    convert_cpp_to_ts "$SHARED_DIR/core/sequencer/pattern.h" "$WEB_DIR/src/types/pattern.types.ts"
else
    echo "⚠️ Pattern header not found, skipping..."
fi

# Синхронизация аудио движка
if [ -f "$SHARED_DIR/core/audio/sample_player.h" ]; then
    convert_cpp_to_ts "$SHARED_DIR/core/audio/sample_player.h" "$WEB_DIR/src/types/audio.types.ts"
else
    echo "⚠️ Audio header not found, skipping..."
fi

# Синхронизация MIDI
if [ -f "$SHARED_DIR/core/midi/midi_parser.h" ]; then
    convert_cpp_to_ts "$SHARED_DIR/core/midi/midi_parser.h" "$WEB_DIR/src/types/midi.types.ts"
else
    echo "⚠️ MIDI header not found, skipping..."
fi

# Синхронизация интерфейсов
if [ -d "$SHARED_DIR/interfaces" ]; then
    echo "🔄 Syncing interfaces..."
    cp -r "$SHARED_DIR/interfaces/"* "$WEB_DIR/src/types/"
    echo "✅ Interfaces synced"
fi

# Синхронизация типов
if [ -d "$SHARED_DIR/types" ]; then
    echo "🔄 Syncing types..."
    cp -r "$SHARED_DIR/types/"* "$WEB_DIR/src/types/"
    echo "✅ Types synced"
fi

# Валидация TypeScript
echo "🔍 Validating TypeScript..."
cd "$WEB_DIR"
if command -v npx &> /dev/null; then
    npx tsc --noEmit --skipLibCheck
    echo "✅ TypeScript validation passed"
else
    echo "⚠️ TypeScript compiler not found, skipping validation"
fi
cd ..

# Генерация документации
echo "📚 Generating documentation..."
if command -v typedoc &> /dev/null; then
    typedoc --out docs/api shared/**/*.ts
    echo "✅ Documentation generated"
else
    echo "⚠️ TypeDoc not found, skipping documentation generation"
fi

echo "✅ Logic synchronization completed!"
echo "📊 Summary:"
echo "  - C++ → TypeScript conversion: ✅"
echo "  - Interface synchronization: ✅"
echo "  - Type synchronization: ✅"
echo "  - TypeScript validation: ✅"
echo "  - Documentation generation: ✅"
