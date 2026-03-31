# 🛠️ ESP32 Groovebox - Hardware Design

## 📋 Overview
Complete hardware design documentation for the ESP32 Groovebox project, including PCB design, schematics, and mechanical specifications.

## 📁 Structure

### 🎯 pcb/ - PCB Design
```
pcb/
├── esp32_groovebox.kicad_pcb    # PCB layout file
├── esp32_groovebox.kicad_pro    # KiCad project settings
├── gerber/                      # Gerber files for production
└── 3d_models/                  # 3D models for visualization
```

**Key Features:**
- **Size:** 200mm x 100mm x 30mm
- **Layers:** 4-layer PCB (2 signal + 2 power)
- **Thickness:** 1.6mm FR4
- **Finish:** HASL (Hot Air Solder Leveling)
- **Silkscreen:** White on green solder mask

### 🔌 schematics/ - Circuit Schematics
```
schematics/
├── power_supply.sch            # 5V/3.3V power regulation
├── audio_section.sch           # I2S DAC and audio output
├── digital_section.sch         # ESP32 and digital logic
└── interface_section.sch       # Controls and displays
```

**Power Supply Section:**
- USB-C power input (5V, 2A)
- AMS1117-3.3 LDO regulator
- Power filtering and protection
- LED power indicator

**Audio Section:**
- PCM5102A I2S DAC
- 3.5mm stereo audio output
- Audio coupling capacitors
- Low-noise power filtering

**Digital Section:**
- ESP32-WROOM-32 main MCU
- 16MB SPI flash
- Level shifters for I/O
- Programming header

**Interface Section:**
- 4x4 matrix keypad
- Rotary encoder with push button
- 128x64 OLED display (SSD1306)
- RGB LED strip controller

### 📦 bom/ - Bill of Materials
```
bom/
├── components.csv              # Main components list
├── alternatives.csv            # Alternative parts
└── suppliers.csv              # Preferred suppliers
```

**Key Components:**
- **MCU:** ESP32-WROOM-32 (WiFi + Bluetooth)
- **DAC:** PCM5102A (I2S 24-bit DAC)
- **Regulator:** AMS1117-3.3 (800mA LDO)
- **Display:** SSD1306 128x64 OLED
- **Encoder:** EC11E rotary encoder
- **Buttons:** 6x6mm tactile switches

### 🏗️ mechanical/ - Mechanical Design
```
mechanical/
├── enclosure.step              # 3D enclosure model
├── panel_layout.dxf           # Front panel layout
└── mounting_holes.dxf         # PCB mounting holes
```

**Enclosure Specifications:**
- **Material:** ABS plastic (black)
- **Dimensions:** 210mm x 110mm x 40mm
- **Wall Thickness:** 2mm
- **Mounting:** 4x M3 screws
- **Ventilation:** Side air vents

**Front Panel Layout:**
- 4x control buttons (PLAY, STOP, REC, SHIFT)
- 1x rotary encoder (parameter control)
- 1x OLED display (128x64)
- 1x 3.5mm audio jack
- Power LED indicator

## 🔧 Design Requirements

### ⚡ Electrical Specifications
- **Power Input:** 5V USB-C, 2A max
- **Current Consumption:** ~500mA typical, 1A peak
- **Operating Voltage:** 3.3V (regulated)
- **Audio Output:** 1V RMS, stereo
- **Sample Rate:** 44.1kHz, 16-bit

### 🌡️ Environmental Specifications
- **Operating Temperature:** 0°C to +50°C
- **Storage Temperature:** -20°C to +70°C
- **Humidity:** 20% to 80% (non-condensing)
- **Altitude:** Sea level to 2000m

### 🔌 Interface Specifications
- **USB:** USB-C 2.0 (power + data)
- **Audio:** 3.5mm TRS stereo jack
- **Display:** I2C OLED (128x64)
- **Controls:** Matrix keypad + encoder
- **LEDs:** WS2812B RGB strip

## 📐 PCB Design Rules

### 🎯 Design Constraints
- **Minimum Trace Width:** 0.15mm (6mil)
- **Minimum Via Size:** 0.6mm drill / 1.0mm pad
- **Minimum Clearance:** 0.1mm (4mil)
- **Annular Ring:** 0.2mm minimum

### 🔋 Power Design
- **Power Traces:** 0.5mm width minimum
- **Ground Plane:** Solid copper pour
- **Decoupling:** 100nF per IC, 10uF bulk
- **Star Ground:** Single point grounding

### 🎵 Audio Design
- **Audio Traces:** 0.15mm width, differential routing
- **Ground Separation:** Analog and digital grounds
- **Shielding:** Ground plane under audio traces
- **Component Placement:** Close to DAC

## 🏭 Manufacturing Notes

### 📋 PCB Production
- **Fabricator:** JLC PCB / PCBWay
- **Quantity:** 10 pieces (prototype)
- **Lead Time:** 5-7 days
- **Cost:** ~$20 for 10 pieces

### 🔧 Assembly
- **Solder Paste:** Type 4 (0.2mm particles)
- **Reflow Profile:** Lead-free (250°C peak)
- **Hand Soldering:** Through-hole components only
- **Testing:** Power-on test, audio test

### 📦 Component Sourcing
- **Primary:** LCSC, Digi-Key, Mouser
- **Alternative:** AliExpress (for prototyping)
- **Lead Time:** 2-4 weeks for some components
- **Cost:** ~$50 for full BOM

## 🔄 Version History

### v1.0 (Current)
- Initial design
- ESP32-WROOM-32 integration
- PCM5102A I2S DAC
- 4x4 matrix keypad
- OLED display
- USB-C power

### Future Plans
- v1.1: Battery power option
- v1.2: SD card storage
- v1.3: MIDI I/O
- v2.0: Custom ASIC

## 📚 Documentation

### 📖 Design Files
- **KiCad:** Open-source PCB design
- **STEP:** 3D mechanical models
- **DXF:** 2D panel layouts
- **CSV:** Bill of materials

### 🔍 Validation
- **Schematic Review:** ERC/DRC checks
- **Simulation:** LTspice for analog circuits
- **Thermal:** PCB temperature analysis
- **EMC:** EMI/EMC considerations

## 📞 Support

### 🤝 Contributing
- Fork the repository
- Create feature branch
- Submit pull request
- Discuss changes in issues

### 🐛 Bug Reports
- Use GitHub issues
- Provide detailed description
- Include photos/screenshots
- Suggest solutions

### 💬 Discussion
- Hardware design questions
- Component alternatives
- Manufacturing suggestions
- Feature requests

---

**Hardware Design Status:** ✅ Complete  
**PCB Status:** ✅ Ready for production  
**BOM Status:** ✅ All parts available  
**Mechanical Status:** ✅ 3D models complete  

*Last updated: 2024-01-01*
