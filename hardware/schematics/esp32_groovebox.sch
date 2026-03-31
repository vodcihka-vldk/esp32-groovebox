EESchema Schematic File Version 4
LIBS:esp32-groovebox-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ESP32 Groovebox - Main Schematic"
Date "2024-01-01"
Rev "1.0"
Comp "GrooveBox Project"
Comment1 "Main System Schematic"
Comment2 "Complete Groovebox Design"
Comment3 "WiFi + Bluetooth + Audio + Controls"
Comment4 ""
$EndDescr
$Sheet
S 1000 1000 2000 1500
U "power_supply.sch" F0 "Power Supply" F1 "power_supply.sch" F2 "ESP32 Groovebox - Power Supply" F3 "5V and 3.3V Regulation"
$EndSheet
$Sheet
S 3500 1000 2000 1500
U "audio_section.sch" F0 "Audio Section" F1 "audio_section.sch" F2 "ESP32 Groovebox - Audio Section" F3 "I2S DAC and Output"
$EndSheet
$Sheet
S 6000 1000 2000 1500
U "digital_section.sch" F0 "Digital Section" F1 "digital_section.sch" F2 "ESP32 Groovebox - Digital Section" F3 "ESP32 and Digital Logic"
$EndSheet
$Sheet
S 8500 1000 2000 1500
U "interface_section.sch" F0 "Interface Section" F1 "interface_section.sch" F2 "ESP32 Groovebox - Interface Section" F3 "User Interface and Controls"
$EndSheet
$Comp
L Connector:Test_Point TP1
U 1 1 5F8B0C80
P 2000 3000
F 0 "TP1" H 2000 3200 50  0000 C CNN
F 1 "5V" H 2000 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 2000 3000 50  0001 C CNN
F 3 "~" H 2000 3000 50  0001 C CNN
	1    2000 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP2
U 1 1 5F8B0C81
P 2200 3000
F 0 "TP2" H 2200 3200 50  0000 C CNN
F 1 "3V3" H 2200 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 2200 3000 50  0001 C CNN
F 3 "~" H 2200 3000 50  0001 C CNN
	1    2200 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP3
U 1 1 5F8B0C82
P 2400 3000
F 0 "TP3" H 2400 3200 50  0000 C CNN
F 1 "GND" H 2400 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 2400 3000 50  0001 C CNN
F 3 "~" H 2400 3000 50  0001 C CNN
	1    2400 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP4
U 1 1 5F8B0C83
P 2600 3000
F 0 "TP4" H 2600 3200 50  0000 C CNN
F 1 "AUDIO_L" H 2600 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 2600 3000 50  0001 C CNN
F 3 "~" H 2600 3000 50  0001 C CNN
	1    2600 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP5
U 1 1 5F8B0C84
P 2800 3000
F 0 "TP5" H 2800 3200 50  0000 C CNN
F 1 "AUDIO_R" H 2800 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 2800 3000 50  0001 C CNN
F 3 "~" H 2800 3000 50  0001 C CNN
	1    2800 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP6
U 1 1 5F8B0C85
P 3000 3000
F 0 "TP6" H 3000 3200 50  0000 C CNN
F 1 "I2C_SDA" H 3000 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 3000 3000 50  0001 C CNN
F 3 "~" H 3000 3000 50  0001 C CNN
	1    3000 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP7
U 1 1 5F8B0C86
P 3200 3000
F 0 "TP7" H 3200 3200 50  0000 C CNN
F 1 "I2C_SCL" H 3200 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 3200 3000 50  0001 C CNN
F 3 "~" H 3200 3000 50  0001 C CNN
	1    3200 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP8
U 1 1 5F8B0C87
P 3400 3000
F 0 "TP8" H 3400 3200 50  0000 C CNN
F 1 "SPI_CLK" H 3400 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 3400 3000 50  0001 C CNN
F 3 "~" H 3400 3000 50  0001 C CNN
	1    3400 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP9
U 1 1 5F8B0C88
P 3600 3000
F 0 "TP9" H 3600 3200 50  0000 C CNN
F 1 "SPI_MOSI" H 3600 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 3600 3000 50  0001 C CNN
F 3 "~" H 3600 3000 50  0001 C CNN
	1    3600 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP10
U 1 1 5F8B0C89
P 3800 3000
F 0 "TP10" H 3800 3200 50  0000 C CNN
F 1 "SPI_MISO" H 3800 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 3800 3000 50  0001 C CNN
F 3 "~" H 3800 3000 50  0001 C CNN
	1    3800 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP11
U 1 1 5F8B0C8A
P 4000 3000
F 0 "TP11" H 4000 3200 50  0000 C CNN
F 1 "SPI_CS" H 4000 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 4000 3000 50  0001 C CNN
F 3 "~" H 4000 3000 50  0001 C CNN
	1    4000 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP12
U 1 1 5F8B0C8B
P 4200 3000
F 0 "TP12" H 4200 3200 50  0000 C CNN
F 1 "UART_TX" H 4200 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 4200 3000 50  0001 C CNN
F 3 "~" H 4200 3000 50  0001 C CNN
	1    4200 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP13
U 1 1 5F8B0C8C
P 4400 3000
F 0 "TP13" H 4400 3200 50  0000 C CNN
F 1 "UART_RX" H 4400 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 4400 3000 50  0001 C CNN
F 3 "~" H 4400 3000 50  0001 C CNN
	1    4400 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP14
U 1 1 5F8B0C8D
P 4600 3000
F 0 "TP14" H 4600 3200 50  0000 C CNN
F 1 "GPIO0" H 4600 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 4600 3000 50  0001 C CNN
F 3 "~" H 4600 3000 50  0001 C CNN
	1    4600 3000
	1    0    0    -1  
$EndComp
$Comp
L Connector:Test_Point TP15
U 1 1 5F8B0C8E
P 4800 3000
F 0 "TP15" H 4800 3200 50  0000 C CNN
F 1 "RESET" H 4800 3200 50  0001 C CNN
F 2 "TestPoint:TestPoint_Pad_1.5x1.5mm" H 4800 3000 50  0001 C CNN
F 3 "~" H 4800 3000 50  0001 C CNN
	1    4800 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	2000 2500 2000 3000
Wire Wire Line
	2200 2500 2200 3000
Wire Wire Line
	2400 2500 2400 3000
Wire Wire Line
	2600 2500 2600 3000
Wire Wire Line
	2800 2500 2800 3000
Wire Wire Line
	3000 2500 3000 3000
Wire Wire Line
	3200 2500 3200 3000
Wire Wire Line
	3400 2500 3400 3000
Wire Wire Line
	3600 2500 3600 3000
Wire Wire Line
	3800 2500 3800 3000
Wire Wire Line
	4000 2500 4000 3000
Wire Wire Line
	4200 2500 4200 3000
Wire Wire Line
	4400 2500 4400 3000
Wire Wire Line
	4600 2500 4600 3000
Wire Wire Line
	4800 2500 4800 3000
Text Label 2000 2900 1    50   ~ 0
5V
Text Label 2200 2900 1    50   ~ 0
3V3
Text Label 2400 2900 1    50   ~ 0
GND
Text Label 2600 2900 1    50   ~ 0
AUDIO_L
Text Label 2800 2900 1    50   ~ 0
AUDIO_R
Text Label 3000 2900 1    50   ~ 0
I2C_SDA
Text Label 3200 2900 1    50   ~ 0
I2C_SCL
Text Label 3400 2900 1    50   ~ 0
SPI_CLK
Text Label 3600 2900 1    50   ~ 0
SPI_MOSI
Text Label 3800 2900 1    50   ~ 0
SPI_MISO
Text Label 4000 2900 1    50   ~ 0
SPI_CS
Text Label 4200 2900 1    50   ~ 0
UART_TX
Text Label 4400 2900 1    50   ~ 0
UART_RX
Text Label 4600 2900 1    50   ~ 0
GPIO0
Text Label 4800 2900 1    50   ~ 0
RESET
Text Notes 2000 3500 0    50   ~ 0
Test Points for Debugging and Programming
Text Notes 2000 3600 0    50   ~ 0
All main signals accessible for testing
Text Notes 2000 3700 0    50   ~ 0
Power, Audio, I2C, SPI, UART
$EndSCHEMATC
