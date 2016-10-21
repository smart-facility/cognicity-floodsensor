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
P 4200 4250
F 0 "P3" H 4200 4500 50  0000 C CNN
F 1 "CONN_01X04" V 4300 4250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x04" H 4200 4250 50  0001 C CNN
F 3 "" H 4200 4250 50  0000 C CNN
	1    4200 4250
	-1   0    0    -1  
$EndComp
Text Notes 3650 4300 0    60   ~ 0
DHT22
$Comp
L R R8
U 1 1 57B344EE
P 4950 4200
F 0 "R8" V 5030 4200 50  0000 C CNN
F 1 "10k" V 4950 4200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4880 4200 50  0001 C CNN
F 3 "" H 4950 4200 50  0000 C CNN
	1    4950 4200
	0    1    1    0   
$EndComp
$Comp
L CONN_01X04 P4
U 1 1 57B344F5
P 4200 5150
F 0 "P4" H 4200 5400 50  0000 C CNN
F 1 "CONN_01X04" V 4300 5150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x04" H 4200 5150 50  0001 C CNN
F 3 "" H 4200 5150 50  0000 C CNN
	1    4200 5150
	-1   0    0    -1  
$EndComp
Text Notes 3500 5200 0    60   ~ 0
HC-SR04
$Comp
L R R9
U 1 1 57B344FF
P 5150 5200
F 0 "R9" V 5050 5200 50  0000 C CNN
F 1 "1k" V 5150 5200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5080 5200 50  0001 C CNN
F 3 "" H 5150 5200 50  0000 C CNN
	1    5150 5200
	0    1    1    0   
$EndComp
$Comp
L R R10
U 1 1 57B34506
P 5150 5300
F 0 "R10" V 5230 5300 50  0000 C CNN
F 1 "1k8" V 5150 5300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5080 5300 50  0001 C CNN
F 3 "" H 5150 5300 50  0000 C CNN
	1    5150 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	4400 4200 4800 4200
Wire Wire Line
	5100 4100 5100 4200
Wire Wire Line
	4400 5000 8000 5000
Wire Wire Line
	4400 5200 5000 5200
Wire Wire Line
	5400 5200 5400 5300
Connection ~ 5400 5200
Wire Wire Line
	4400 4100 8000 4100
Wire Wire Line
	4400 4400 4900 4400
Wire Wire Line
	4400 5100 8000 5100
Wire Wire Line
	5300 5200 8000 5200
Wire Wire Line
	4400 5300 5000 5300
Wire Wire Line
	5400 5300 5300 5300
Text HLabel 8000 5100 2    60   Input ~ 0
TRIG
Text HLabel 8000 5200 2    60   Input ~ 0
ECHO
Text HLabel 8000 4300 2    60   Input ~ 0
DHT
Wire Wire Line
	4700 4200 4700 4300
Wire Wire Line
	4700 4300 8000 4300
Connection ~ 4700 4200
Connection ~ 4900 5300
Text HLabel 4300 2300 0    60   Input ~ 0
5Vsensor
Connection ~ 5100 4100
Wire Wire Line
	4900 4400 4900 6100
$Comp
L HT73xx U5
U 1 1 57B665D3
P 5800 2350
F 0 "U5" H 5950 2154 50  0000 C CNN
F 1 "HT7333" H 5800 2550 50  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT89-3_Housing" H 5800 2350 50  0001 C CNN
F 3 "" H 5800 2350 50  0000 C CNN
	1    5800 2350
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 57B665DA
P 5100 2650
F 0 "C2" H 5125 2750 50  0000 L CNN
F 1 "10u" H 5125 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 5138 2500 50  0001 C CNN
F 3 "" H 5100 2650 50  0000 C CNN
	1    5100 2650
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 57B665E1
P 6400 2650
F 0 "C3" H 6425 2750 50  0000 L CNN
F 1 "10u" H 6425 2550 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805" H 6438 2500 50  0001 C CNN
F 3 "" H 6400 2650 50  0000 C CNN
	1    6400 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2300 5400 2300
Wire Wire Line
	5100 2300 5100 2500
Connection ~ 5100 2300
Wire Wire Line
	5100 2800 5100 2900
Wire Wire Line
	4300 2900 7300 2900
Connection ~ 5800 2900
Wire Wire Line
	6400 2900 6400 2800
Connection ~ 6400 2900
Wire Wire Line
	6400 2500 6400 2300
Connection ~ 6400 2300
Wire Wire Line
	5800 2600 5800 2900
Connection ~ 5100 2900
Text GLabel 4300 2900 0    60   Input ~ 0
GND
Wire Wire Line
	6200 2300 7300 2300
Text Label 4550 2900 0    60   ~ 0
GND
Text Label 4550 2300 0    60   ~ 0
5V
Text Label 6050 5000 0    60   ~ 0
5V
Text Label 6100 4100 0    60   ~ 0
VCC
Text Label 6750 2300 0    60   ~ 0
VCC
Wire Wire Line
	4900 6100 8000 6100
Text Label 6050 6100 0    60   ~ 0
GND
Wire Wire Line
	4400 4300 4550 4300
Wire Wire Line
	4550 4300 4550 4400
Connection ~ 4550 4400
Text HLabel 4300 1900 0    60   Input ~ 0
PiVCC
$Comp
L CONN_01X02 P13
U 1 1 580A18FA
P 6450 1700
F 0 "P13" H 6450 1850 50  0000 C CNN
F 1 "CONN_01X02" V 6550 1700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 6450 1700 50  0001 C CNN
F 3 "" H 6450 1700 50  0000 C CNN
	1    6450 1700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6500 1900 6500 2300
Connection ~ 6500 2300
Wire Wire Line
	6400 1900 4300 1900
$EndSCHEMATC
