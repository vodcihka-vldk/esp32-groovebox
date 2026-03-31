EESchema Schematic File Version 4
LIBS:esp32_groovebox-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ESP32 Groovebox - Power Supply"
Date "2024-01-01"
Rev "1.0"
Comp "GrooveBox Project"
Comment1 "Power Supply Section"
Comment2 "5V and 3.3V Regulation"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:USB_C_Receptacle_USB2.0 J1
U 1 1 5F8B0C1A
P 1500 2000
F 0 "J1" H 1557 3267 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 1557 3176 50  0000 C CNN
F 2 "Connector_USB:USB_C_Receptacle_HRO_TYPE-C-31-M-12" H 1650 2000 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 1650 2000 50  0001 C CNN
	1    1500 2000
	1    0    0    -1  
$EndComp
$Comp
L Regulator_Linear:AMS1117-3.3 U1
U 1 1 5F8B0C1B
P 3500 1600
F 0 "U1" H 3500 1842 50  0000 C CNN
F 1 "AMS1117-3.3" H 3500 1751 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 3500 1800 50  0001 C CNN
F 3 "http://www.advanced-monolithic.com/pdf/ds1117.pdf" H 3500 1600 50  0001 C CNN
	1    3500 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C1
U 1 1 5F8B0C1C
P 2500 1800
F 0 "C1" H 2592 1846 50  0000 L CNN
F 1 "10uF" H 2592 1755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2500 1800 50  0001 C CNN
F 3 "~" H 2500 1800 50  0001 C CNN
	1    2500 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 5F8B0C1D
P 4500 1800
F 0 "C2" H 4592 1846 50  0000 L CNN
F 1 "22uF" H 4592 1755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 4500 1800 50  0001 C CNN
F 3 "~" H 4500 1800 50  0001 C CNN
	1    4500 1800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR01
U 1 1 5F8B0C1E
P 2500 1400
F 0 "#PWR01" H 2500 1250 50  0001 C CNN
F 1 "+5V" H 2515 1573 50  0000 C CNN
F 2 "" H 2500 1400 50  0001 C CNN
F 3 "" H 2500 1400 50  0001 C CNN
	1    2500 1400
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR02
U 1 1 5F8B0C1F
P 4500 1400
F 0 "#PWR02" H 4500 1250 50  0001 C CNN
F 1 "+3V3" H 4515 1573 50  0000 C CNN
F 2 "" H 4500 1400 50  0001 C CNN
F 3 "" H 4500 1400 50  0001 C CNN
	1    4500 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5F8B0C20
P 3500 2100
F 0 "#PWR03" H 3500 1850 50  0001 C CNN
F 1 "GND" H 3505 1927 50  0000 C CNN
F 2 "" H 3500 2100 50  0001 C CNN
F 3 "" H 3500 2100 50  0001 C CNN
	1    3500 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 2700 1500 2800
Wire Wire Line
	1500 2800 2500 2800
Wire Wire Line
	2500 2800 2500 1900
Wire Wire Line
	3500 1900 3500 2100
Wire Wire Line
	4500 1900 4500 2800
Wire Wire Line
	4500 2800 3500 2800
Wire Wire Line
	3500 2800 3500 2100
Connection ~ 3500 2100
Wire Wire Line
	2100 1600 2500 1600
Wire Wire Line
	2500 1600 2500 1700
Wire Wire Line
	2500 1600 2500 1400
Connection ~ 2500 1600
Wire Wire Line
	2500 1600 3200 1600
Wire Wire Line
	3800 1600 4500 1600
Wire Wire Line
	4500 1600 4500 1700
Wire Wire Line
	4500 1600 4500 1400
Connection ~ 4500 1600
$Comp
L Device:LED_Small D1
U 1 1 5F8B0C21
P 5500 1600
F 0 "D1" H 5500 1393 50  0000 C CNN
F 1 "PWR_LED" H 5500 1484 50  0000 C CNN
F 2 "LED_SMD:LED_0603_1608Metric" H 5500 1600 50  0001 C CNN
F 3 "~" H 5500 1600 50  0001 C CNN
	1    5500 1600
	-1   0    0    1   
$EndComp
$Comp
L Device:R_Small R1
U 1 1 5F8B0C22
P 6000 1600
F 0 "R1" V 5804 1600 50  0000 C CNN
F 1 "1K" V 5895 1600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6000 1600 50  0001 C CNN
F 3 "~" H 6000 1600 50  0001 C CNN
	1    6000 1600
	0    1    1    0   
$EndComp
Wire Wire Line
	4500 1600 5400 1600
Wire Wire Line
	5600 1600 5900 1600
Wire Wire Line
	6100 1600 6100 1400
Wire Wire Line
	6100 1400 4500 1400
Connection ~ 4500 1400
$EndSCHEMATC
