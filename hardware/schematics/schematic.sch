EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:linear
LIBS:regul
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:ESP8266
LIBS:GY512 (2)
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L NodeMCU1.0(ESP-12E) U?
U 1 1 5A1585CF
P 3350 3850
F 0 "U?" H 3350 4700 60  0000 C CNN
F 1 "NodeMCU1.0(ESP-12E)" H 3350 3000 60  0000 C CNN
F 2 "" H 2750 3000 60  0000 C CNN
F 3 "" H 2750 3000 60  0000 C CNN
	1    3350 3850
	-1   0    0    1   
$EndComp
$Comp
L GY512 U?
U 1 1 5A158682
P 1500 3500
F 0 "U?" H 1500 3400 50  0000 C CNN
F 1 "GY512" H 1500 3600 50  0000 C CNN
F 2 "MODULE" H 1500 3500 50  0001 C CNN
F 3 "DOCUMENTATION" H 1500 3500 50  0001 C CNN
	1    1500 3500
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 5A158823
P 1950 4550
F 0 "D?" H 1950 4650 50  0000 C CNN
F 1 "LED" H 1950 4450 50  0000 C CNN
F 2 "" H 1950 4550 50  0001 C CNN
F 3 "" H 1950 4550 50  0001 C CNN
	1    1950 4550
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 5A158887
P 2300 4550
F 0 "R?" V 2380 4550 50  0000 C CNN
F 1 "R" V 2300 4550 50  0000 C CNN
F 2 "" V 2230 4550 50  0001 C CNN
F 3 "" H 2300 4550 50  0001 C CNN
	1    2300 4550
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR?
U 1 1 5A158969
P 1650 4650
F 0 "#PWR?" H 1650 4400 50  0001 C CNN
F 1 "GND" H 1650 4500 50  0000 C CNN
F 2 "" H 1650 4650 50  0001 C CNN
F 3 "" H 1650 4650 50  0001 C CNN
	1    1650 4650
	1    0    0    -1  
$EndComp
$Comp
L Battery BT?
U 1 1 5A158A11
P 4900 3200
F 0 "BT?" H 5000 3300 50  0000 L CNN
F 1 "Battery" H 5000 3200 50  0000 L CNN
F 2 "" V 4900 3260 50  0001 C CNN
F 3 "" V 4900 3260 50  0001 C CNN
	1    4900 3200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 5A158A48
P 4650 3400
F 0 "#PWR?" H 4650 3150 50  0001 C CNN
F 1 "GND" H 4650 3250 50  0000 C CNN
F 2 "" H 4650 3400 50  0001 C CNN
F 3 "" H 4650 3400 50  0001 C CNN
	1    4650 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2100 3150 2550 3150
Wire Wire Line
	2550 3250 2100 3250
Wire Wire Line
	2100 3350 2450 3350
Wire Wire Line
	2450 3350 2450 4350
Wire Wire Line
	2450 4350 2550 4350
Wire Wire Line
	2550 4450 2350 4450
Wire Wire Line
	2350 4450 2350 3450
Wire Wire Line
	2350 3450 2100 3450
Wire Wire Line
	2100 3750 2150 3750
Wire Wire Line
	2150 3750 2150 3250
Connection ~ 2150 3250
Wire Wire Line
	2150 4550 2100 4550
Wire Wire Line
	2450 4550 2550 4550
Wire Wire Line
	1650 4650 1650 4550
Wire Wire Line
	1650 4550 1800 4550
Wire Wire Line
	4300 3400 4900 3400
Wire Wire Line
	4150 3250 4300 3250
Wire Wire Line
	4300 3250 4300 3400
Connection ~ 4650 3400
$Comp
L TS5A3159DCK U?
U 1 1 5A158CEE
P 4500 2750
F 0 "U?" H 4375 2935 50  0000 L CNN
F 1 "Switch" H 4375 2865 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-363_SC-70-6" H 4500 2450 50  0001 C CNN
F 3 "" H 4500 2750 50  0001 C CNN
	1    4500 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 3150 4150 2850
Wire Wire Line
	4150 2850 4200 2850
Wire Wire Line
	4800 2950 4900 2950
Wire Wire Line
	4900 2950 4900 3000
$EndSCHEMATC
