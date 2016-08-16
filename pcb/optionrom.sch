EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
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
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:wbt-device
LIBS:flood-hat-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
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
L 24C16 U1
U 1 1 57B3028F
P 4600 3500
F 0 "U1" H 4750 3850 50  0000 C CNN
F 1 "CAT24C32" H 4800 3150 50  0000 C CNN
F 2 "SMD_Packages:SOIC-8-N" H 4600 3500 50  0001 C CNN
F 3 "" H 4600 3500 50  0000 C CNN
	1    4600 3500
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 57B30296
P 5700 3150
F 0 "R2" V 5780 3150 50  0000 C CNN
F 1 "3k9" V 5700 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5630 3150 50  0001 C CNN
F 3 "" H 5700 3150 50  0000 C CNN
	1    5700 3150
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 57B3029D
P 5900 3150
F 0 "R3" V 5980 3150 50  0000 C CNN
F 1 "3k9" V 5900 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5830 3150 50  0001 C CNN
F 3 "" H 5900 3150 50  0000 C CNN
	1    5900 3150
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 57B302A4
P 5500 3150
F 0 "R1" V 5580 3150 50  0000 C CNN
F 1 "1k" V 5500 3150 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5430 3150 50  0001 C CNN
F 3 "" H 5500 3150 50  0000 C CNN
	1    5500 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3400 3900 3400
Wire Wire Line
	3700 3500 3900 3500
Connection ~ 3700 3400
Connection ~ 3700 3500
Wire Wire Line
	4600 3000 4600 2800
Wire Wire Line
	6900 3600 5300 3600
Wire Wire Line
	5300 3700 6900 3700
Wire Wire Line
	5500 2800 5500 3000
Connection ~ 5500 2800
Connection ~ 5700 2800
Wire Wire Line
	5300 3400 5500 3400
Wire Wire Line
	5500 3300 5500 4000
Wire Wire Line
	5700 3300 5700 3600
Connection ~ 5700 3600
Wire Wire Line
	5900 3300 5900 3700
Connection ~ 5900 3700
Wire Wire Line
	5700 2800 5700 3000
Wire Wire Line
	5900 2800 5900 3000
Wire Wire Line
	3900 3300 3700 3300
Wire Wire Line
	3700 3300 3700 4300
Connection ~ 5500 3400
Wire Wire Line
	4600 4300 4600 4000
Connection ~ 4600 4300
Wire Wire Line
	3100 2800 5900 2800
Text HLabel 6900 3600 2    60   Input ~ 0
SCL
Text HLabel 6900 3700 2    60   Input ~ 0
SDA
$Comp
L C C1
U 1 1 57B313A1
P 3100 3700
F 0 "C1" H 3125 3800 50  0000 L CNN
F 1 "100n" H 3125 3600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 3138 3550 50  0001 C CNN
F 3 "" H 3100 3700 50  0000 C CNN
	1    3100 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 3850 3100 4300
Connection ~ 3700 4300
Wire Wire Line
	3100 3550 3100 2800
Connection ~ 4600 2800
Text GLabel 3100 4300 0    60   Input ~ 0
GND
Text HLabel 3100 2800 0    60   Input ~ 0
PIVCC
Wire Wire Line
	3100 4300 4600 4300
Text HLabel 6900 4000 2    60   Input ~ 0
WP
Wire Wire Line
	5500 4000 6900 4000
$EndSCHEMATC
