# 🔧 KiCad Files for ESP32 Groovebox

## 📋 Overview
Complete KiCad project files for ESP32 Groovebox hardware design.

## 🎯 Files Available

### ✅ Working Files (Recommended)
- **esp32_groovebox_working.kicad_pro** - Project file (KiCad 7.x compatible)
- **esp32_groovebox_working.kicad_pcb** - PCB layout file (KiCad 7.x compatible)

### ⚠️ Original Files (Legacy)
- **esp32_groovebox.kicad_pro** - Original project file (KiCad 6.x)
- **esp32_groovebox.kicad_pcb** - Original PCB layout file (KiCad 6.x)
- **esp32_groovebox_v2.kicad_pro** - Updated project file (KiCad 7.x)
- **esp32_groovebox_v2.kicad_pcb** - Updated PCB layout file (KiCad 7.x)

## 🚀 Quick Start

### For KiCad 7.x and newer:
1. Open **esp32_groovebox_working.kicad_pro**
2. The PCB file will be loaded automatically
3. Start editing or generate Gerber files

### For KiCad 6.x:
1. Open **esp32_groovebox.kicad_pro**
2. Use the legacy PCB file
3. May need to upgrade to KiCad 7.x for full compatibility

## 🔧 File Structure

### Working Files (Recommended)
```
esp32_groovebox_working.kicad_pro  # Project settings
esp32_groovebox_working.kicad_pcb  # PCB layout with components
```

### Components Included:
- **U2**: PCM5102A I2S DAC (TSSOP-20 package)
- **C1**: 10uF capacitor (0603 package)
- PCB outline: 60mm x 60mm
- Proper layer stackup for 2-layer PCB
- Net classes for Power, Audio, and Default signals

## 📐 PCB Specifications

### Physical:
- **Size**: 60mm x 60mm
- **Layers**: 2 (Top + Bottom)
- **Thickness**: 1.6mm FR4
- **Finish**: HASL (default)

### Design Rules:
- **Minimum trace width**: 0.15mm (Audio), 0.25mm (Default), 0.5mm (Power)
- **Minimum via size**: 0.6mm (Audio), 0.8mm (Default), 1.0mm (Power)
- **Clearance**: 0.1mm (Audio), 0.2mm (Default), 0.15mm (Power)

### Layers:
- **F.Cu**: Top copper layer
- **B.Cu**: Bottom copper layer
- **F.SilkS**: Top silkscreen
- **B.SilkS**: Bottom silkscreen
- **F.Mask**: Top solder mask
- **B.Mask**: Bottom solder mask
- **Edge.Cuts**: Board outline

## 🎯 Net Classes

### Default:
- **Track width**: 0.25mm
- **Via size**: 0.8mm / 0.4mm drill
- **Clearance**: 0.2mm

### Power:
- **Track width**: 0.5mm
- **Via size**: 1.0mm / 0.5mm drill
- **Clearance**: 0.15mm

### Audio:
- **Track width**: 0.15mm
- **Via size**: 0.6mm / 0.3mm drill
- **Clearance**: 0.1mm

## 🔍 Testing the Files

### To verify they work:
1. Open **esp32_groovebox_working.kicad_pro** in KiCad
2. Check that PCB loads without errors
3. Verify components are visible
4. Check that nets are properly defined
5. Test DRC (Design Rule Check)

### Common Issues:
- **File not found**: Make sure both .kicad_pro and .kicad_pcb files are in the same directory
- **Version mismatch**: Use KiCad 7.x or newer for working files
- **Missing libraries**: Install standard KiCad libraries

## 📦 Exporting for Production

### Gerber Files:
1. Open project in KiCad
2. Go to File → Plot
3. Select all layers
4. Enable "Gerber X2 format"
5. Click "Plot"

### Drill Files:
1. Go to File → Plot
2. Select "Generate Drill Files"
3. Choose "Excellon format"
4. Click "Generate Drill Files"

## 🛠️ Next Steps

### For Production:
1. Complete the PCB layout
2. Run DRC check
3. Generate Gerber files
4. Send to manufacturer

### For Development:
1. Add remaining components (ESP32, connectors, etc.)
2. Route all signals
3. Add test points
4. Verify design rules

## 📞 Support

### If files don't open:
1. Check KiCad version (7.x recommended)
2. Verify both files are present
3. Try opening the .kicad_pcb file directly
4. Check for error messages in KiCad console

### For design questions:
- Refer to the main README.md
- Check the assembly guide
- Review the BOM for component specifications

---

**Last updated: 2024-01-01**  
**Compatible with KiCad 7.x and newer**
