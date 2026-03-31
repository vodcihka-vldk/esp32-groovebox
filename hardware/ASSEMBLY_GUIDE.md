# 🔧 ESP32 Groovebox - Assembly Guide

## 📋 Overview
Complete step-by-step assembly instructions for ESP32 Groovebox hardware.

## 🛠️ Required Tools
- Soldering iron (temperature controlled)
- Solder wire (0.6mm diameter)
- Flux (rosin core)
- Tweezers (fine tip)
- Multimeter
- PCB holder
- Magnifying glass or microscope
- Isopropyl alcohol
- Cleaning brush

## 📦 Components Checklist

### 🔌 Power Supply Section
- [ ] J1: USB-C Receptacle (HRO TYPE-C-31-M-12)
- [ ] U1: AMS1117-3.3 LDO Regulator
- [ ] C1: 10uF 0603 capacitor
- [ ] C2: 22uF 0603 capacitor
- [ ] R1: 1K 0603 resistor
- [ ] D1: Power LED 0603

### 🎵 Audio Section
- [ ] U2: PCM5102A I2S DAC
- [ ] J2: 3.5mm Audio Jack (WQP-PJ398M)
- [ ] C3: 100nF 0402 capacitor
- [ ] C4: 100nF 0402 capacitor
- [ ] C5: 10uF 0603 capacitor
- [ ] C6: 10uF 0603 capacitor
- [ ] C7: 10uF 0603 capacitor
- [ ] C8: 10uF 0603 capacitor
- [ ] R2: 100K 0402 resistor

### 💻 Digital Section
- [ ] U3: ESP32-WROOM-32
- [ ] U4: W25Q32JVSS Flash (16MB)
- [ ] Y1: 26MHz Crystal
- [ ] C9: 100nF 0402 capacitor
- [ ] C10: 100nF 0402 capacitor
- [ ] C11: 100nF 0402 capacitor
- [ ] C12: 10uF 0603 capacitor
- [ ] C13: 12pF 0402 capacitor
- [ ] C14: 12pF 0402 capacitor
- [ ] R3: 10K 0402 resistor
- [ ] R4: 10K 0402 resistor
- [ ] J3: Programming Header (6-pin)

### 🎮 Interface Section
- [ ] U5: SSD1306 128x64 OLED
- [ ] ENC1: EC11E Rotary Encoder
- [ ] SW1-SW4: 6x6mm Tactile Switches
- [ ] D2-D4: Status LEDs 0603
- [ ] R5-R7: 1K 0603 resistors
- [ ] R8-R12: 10K 0402 resistors
- [ ] J4: LED Strip Connector (4-pin)

## 🔧 Assembly Steps

### 1. Power Supply Section
1. **Install USB-C Connector (J1)**
   - Solder to PCB, ensure proper alignment
   - Check for shorts with multimeter

2. **Install LDO Regulator (U1)**
   - Apply thermal paste to tab
   - Solder pins, then tab
   - Verify 3.3V output

3. **Install Power Components**
   - Solder C1, C2 (observe polarity)
   - Solder R1 and D1
   - Test power LED operation

### 2. Digital Section
1. **Install ESP32 (U3)**
   - Apply flux to pads
   - Solder carefully, avoid bridges
   - Verify all connections

2. **Install Flash Memory (U4)**
   - Align pin 1 indicator
   - Solder all 8 pins
   - Check for shorts

3. **Install Crystal and Load Caps**
   - Solder Y1 crystal
   - Install C13, C14 close to crystal
   - Verify oscillator startup

4. **Install Decoupling Capacitors**
   - Solder C9, C10 near ESP32
   - Install C11, C12 near flash
   - Ensure solid ground connections

### 3. Audio Section
1. **Install DAC (U2)**
   - Align pin 1 marker
   - Solder all 20 pins carefully
   - Check for solder bridges

2. **Install Audio Jack (J2)**
   - Solder mechanical supports first
   - Solder electrical connections
   - Test mechanical fit

3. **Install Audio Components**
   - Solder C3-C8 capacitors
   - Install R2 pull-up resistor
   - Verify audio signal path

### 4. Interface Section
1. **Install OLED Display (U5)**
   - Solder 4-pin header
   - Connect I2C lines (SDA/SCL)
   - Test display operation

2. **Install Rotary Encoder (ENC1)**
   - Solder mechanical pins first
   - Solder electrical connections
   - Test encoder operation

3. **Install Control Switches**
   - Solder SW1-SW4 switches
   - Install pull-up resistors R8-R12
   - Test button functionality

4. **Install Status LEDs**
   - Solder D2-D4 LEDs
   - Install current limiting R5-R7
   - Test LED operation

5. **Install LED Strip Connector**
   - Solder J4 4-pin header
   - Connect to GPIO pin
   - Test LED strip functionality

### 5. Final Assembly
1. **Programming Header**
   - Solder J3 programming header
   - Verify all connections
   - Test programming interface

2. **Test Points**
   - Verify all test points accessible
   - Check continuity
   - Label important signals

3. **Final Inspection**
   - Visual inspection for defects
   - Check solder joint quality
   - Verify component alignment

## 🔍 Testing Procedures

### Power-On Test
1. **Initial Power**
   - Connect USB-C power
   - Check for smoke/overheating
   - Verify 3.3V output

2. **Basic Functionality**
   - Check power LED
   - Test OLED display
   - Verify button responses

### Functional Test
1. **Audio Test**
   - Connect headphones
   - Play test tone
   - Check audio quality

2. **Interface Test**
   - Test all buttons
   - Verify encoder operation
   - Check LED indicators

3. **Communication Test**
   - Test programming interface
   - Verify UART communication
   - Check WiFi/Bluetooth

## 🚨 Troubleshooting

### Common Issues
1. **No Power**
   - Check USB-C connection
   - Verify LDO output
   - Check for shorts

2. **No Display**
   - Verify I2C connections
   - Check 3.3V power
   - Test with I2C scanner

3. **No Audio**
   - Check DAC connections
   - Verify I2S signals
   - Test audio jack

4. **Button Issues**
   - Check pull-up resistors
   - Verify GPIO connections
   - Test with multimeter

### Debug Tools
- Oscilloscope for signal analysis
- Logic analyzer for digital debugging
- Power supply for current measurement
- Hot air rework for component replacement

## 📋 Final Checklist
- [ ] All components installed
- [ ] No solder bridges
- [ ] Power LED working
- [ ] OLED display functional
- [ ] Audio output working
- [ ] All buttons responding
- [ ] Encoder working
- [ ] Programming interface functional
- [ ] LED strip working
- [ ] WiFi/Bluetooth functional
- [ ] Final inspection complete

## 📞 Support
- Check schematic for proper connections
- Verify component datasheets
- Use test points for debugging
- Consult assembly documentation

---

**Assembly Time:** 2-4 hours for experienced builders  
**Difficulty:** Intermediate (SMD soldering required)  
**Tools Required:** Basic electronics workshop  

*Last updated: 2024-01-01*
