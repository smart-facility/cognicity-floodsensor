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
Sheet 4 5
Title ""
Date "2016-08-16"
Rev "Sensor Ports"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X04 P3
U 1 1 57B344E3
P 4200 3050
F 0 "P3" H 4200 3300 50  0000 C CNN
F 1 "CONN_01X04" V 4300 3050 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x04" H 4200 3050 50  0001 C CNN
F 3 "" H 4200 3050 50  0000 C CNN
	1    4200 3050
	-1   0    0    -1  
$EndComp
Text Notes 3650 3100 0    60   ~ 0
DHT22
$Comp
L R R8
U 1 1 57B344EE
P 4950 3000
F 0 "R8" V 5030 3000 50  0000 C CNN
F 1 "10k" V 4950 3000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4880 3000 50  0001 C CNN
F 3 "" H 4950 3000 50  0000 C CNN
	1    4950 3000
	0    1    1    0   
$EndComp
$Comp
L CONN_01X04 P4
U 1 1 57B344F5
P 4200 3950
F 0 "P4" H 4200 4200 50  0000 C CNN
F 1 "CONN_01X04" V 4300 3950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x04" H 4200 3950 50  0001 C CNN
F 3 "" H 4200 3950 50  0000 C CNN
	1    4200 3950
	-1   0    0    -1  
$EndComp
Text Notes 3500 4000 0    60   ~ 0
HC-SR04
$Comp
L R R9
U 1 1 57B344FF
P 5150 4000
F 0 "R9" V 5050 4000 50  0000 C CNN
F 1 "1k" V 5150 4000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5080 4000 50  0001 C CNN
F 3 "" H 5150 4000 50  0000 C CNN
	1    5150 4000
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 57B34506
P 5150 4100
F 0 "R10" V 5230 4100 50  0000 C CNN
F 1 "1k8" V 5150 4100 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5080 4100 50  0001 C CNN
F 3 "" H 5150 4100 50  0000 C CNN
	1    5150 4100
	0    1    1    0   
$EndComp
Wire Wire Line
	4400 3000 4800 3000
Wire Wire Line
	5100 2900 5100 3000
Wire Wire Line
	4400 3800 8000 3800
Wire Wire Line
	4400 4000 5000 4000
Wire Wire Line
	5400 4000 5400 4100
Connection ~ 5400 4000
Wire Wire Line
	4400 2900 8000 2900
Wire Wire Line
	4400 3200 4900 3200
Wire Wire Line
	4400 3900 8000 3900
Wire Wire Line
	5300 4000 8000 4000
Wire Wire Line
	4400 4100 5000 4100
Wire Wire Line
	5400 4100 5300 4100
Text HLabel 8000 3900 2    60   Input ~ 0
TRIG
Text HLabel 8000 4000 2    60   Input ~ 0
ECHO
Text HLabel 8000 3100 2    60   Input ~ 0
DHT
Wire Wire Line
	4700 3000 4700 3100
Wire Wire Line
	4700 3100 8000 3100
Connection ~ 4700 3000
Connection ~ 4900 4100
Text HLabel 4300 1100 0    60   Input ~ 0
5Vsensor
Connection ~ 5100 2900
Wire Wire Line
	4900 3200 4900 4900
$Comp
L HT73xx U5
U 1 1 57B665D3
P 5800 1150
F 0 "U5" H 5950 954 50  0000 C CNN
F 1 "HT7333" H 5800 1350 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT89-3_Housing" H 5800 1150 50  0001 C CNN
F 3 "" H 5800 1150 50  0000 C CNN
	1    5800 1150
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 57B665DA
P 5100 1450
F 0 "C2" H 5125 1550 50  0000 L CNN
F 1 "10u" H 5125 1350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5138 1300 50  0001 C CNN
F 3 "" H 5100 1450 50  0000 C CNN
	1    5100 1450
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 57B665E1
P 6400 1450
F 0 "C3" H 6425 1550 50  0000 L CNN
F 1 "10u" H 6425 1350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6438 1300 50  0001 C CNN
F 3 "" H 6400 1450 50  0000 C CNN
	1    6400 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 1100 5400 1100
Wire Wire Line
	5100 1100 5100 1300
Connection ~ 5100 1100
Wire Wire Line
	5100 1600 5100 1700
Wire Wire Line
	4300 1700 7300 1700
Connection ~ 5800 1700
Wire Wire Line
	6400 1700 6400 1600
Connection ~ 6400 1700
Wire Wire Line
	6400 1300 6400 1100
Connection ~ 6400 1100
Wire Wire Line
	5800 1400 5800 1700
Connection ~ 5100 1700
Text GLabel 4300 1700 0    60   Input ~ 0
GND
Wire Wire Line
	6200 1100 7300 1100
Text Label 4550 1700 0    60   ~ 0
GND
Text Label 4550 1100 0    60   ~ 0
5V
Text Label 6050 3800 0    60   ~ 0
5V
Text Label 6100 2900 0    60   ~ 0
VCC
Text Label 6750 1100 0    60   ~ 0
VCC
Wire Wire Line
	4900 4900 8000 4900
Text Label 6050 4900 0    60   ~ 0
GND
Wire Wire Line
	4400 3100 4550 3100
Wire Wire Line
	4550 3100 4550 3200
Connection ~ 4550 3200
$EndSCHEMATC
