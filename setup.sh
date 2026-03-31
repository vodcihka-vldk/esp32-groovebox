#!/bin/bash

# ESP32 Groovebox Development Setup Script

echo "🎵 ESP32 Groovebox Development Setup"
echo "===================================="

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "❌ PlatformIO not found. Installing..."
    
    # Install PlatformIO CLI
    pip install platformio
    
    # Install VS Code extension (if VS Code is available)
    if command -v code &> /dev/null; then
        echo "📦 Installing VS Code PlatformIO extension..."
        code --install-extension platformio.platformio-ide
    fi
else
    echo "✅ PlatformIO found"
fi

# Check Python dependencies
echo "📦 Checking Python dependencies..."

# Required packages
packages=("numpy" "wave")

for package in "${packages[@]}"; do
    if python -c "import $package" &> /dev/null; then
        echo "✅ $package is installed"
    else
        echo "📦 Installing $package..."
        pip install $package
    fi
done

# Create sample files
echo "🎵 Generating sample files..."
cd tools
python generate_samples.py
cd ..

# Convert samples to C arrays
echo "🔄 Converting samples to C arrays..."
if [ -d "samples" ]; then
    mkdir -p src/samples
    
    for wav_file in samples/*.wav; do
        if [ -f "$wav_file" ]; then
            filename=$(basename "$wav_file" .wav)
            echo "Converting $filename..."
            python tools/wav2c.py "$wav_file" "src/samples/${filename}.h" "${filename}_data"
        fi
    done
    
    echo "✅ Sample conversion complete"
else
    echo "❌ samples directory not found"
fi

# Create samples header file
echo "📝 Creating samples header..."
cat > src/samples.h << 'EOF'
#ifndef SAMPLES_H
#define SAMPLES_H

#include <stdint.h>

// External sample data declarations
extern const uint16_t kick_data_length;
extern const int16_t kick_data[];

extern const uint16_t snare_data_length;
extern const int16_t snare_data[];

extern const uint16_t hihat_closed_data_length;
extern const int16_t hihat_closed_data[];

extern const uint16_t hihat_open_data_length;
extern const int16_t hihat_open_data[];

extern const uint16_t tom_data_length;
extern const int16_t tom_data[];

extern const uint16_t clap_data_length;
extern const int16_t clap_data[];

extern const uint16_t crash_data_length;
extern const int16_t crash_data[];

// Sample bank structure
struct Sample {
    const int16_t* data;
    uint32_t length;
    const char* name;
};

// Built-in sample bank
extern const Sample builtin_samples[8];

#endif // SAMPLES_H
EOF

echo "✅ Setup complete!"
echo ""
echo "🚀 Next steps:"
echo "1. Connect ESP32 hardware according to README.md"
echo "2. Open project in VS Code with PlatformIO"
echo "3. Upload to ESP32"
echo "4. Monitor serial output"
echo ""
echo "📚 For detailed instructions, see README.md"
