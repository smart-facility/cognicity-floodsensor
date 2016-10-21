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
Sheet 3 5
Title "Power Control circuitry"
Date "2016-08-16"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X02 P2
U 1 1 57B323EB
P 2000 2850
F 0 "P2" H 2000 3000 50  0000 C CNN
F 1 "CONN_01X02" V 2100 2850 50  0000 C CNN
F 2 "Terminal_Blocks:TerminalBlock_Pheonix_MKDS1.5-2pol" H 2000 2850 50  0001 C CNN
F 3 "" H 2000 2850 50  0000 C CNN
	1    2000 2850
	-1   0    0    -1  
$EndComp
Text Notes 1550 2900 0    60   ~ 0
5V in
$Comp
L Q_PMOS_GSD Q1
U 1 1 57B323F3
P 3300 2900
F 0 "Q1" V 3500 3100 50  0000 R CNN
F 1 "AO3401" V 3550 2950 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 3500 3000 50  0001 C CNN
F 3 "" H 3300 2900 50  0000 C CNN
	1    3300 2900
	0    -1   -1   0   
$EndComp
$Comp
L PNP2 U2
U 1 1 57B323FA
P 3300 3450
F 0 "U2" H 3300 3700 39  0000 C CNN
F 1 "DMMT5401" H 3300 3550 39  0000 C CNN
F 2 "TO_SOT_Packages_SMD:TSOT-6-MK06A" H 3150 3600 39  0001 C CNN
F 3 "" H 3150 3600 39  0000 C CNN
	1    3300 3450
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 57B32401
P 2900 4250
F 0 "R4" V 2980 4250 50  0000 C CNN
F 1 "10k" V 2900 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 2830 4250 50  0001 C CNN
F 3 "" H 2900 4250 50  0000 C CNN
	1    2900 4250
	-1   0    0    1   
$EndComp
$Comp
L R R5
U 1 1 57B32408
P 3700 4250
F 0 "R5" V 3780 4250 50  0000 C CNN
F 1 "47k" V 3700 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 3630 4250 50  0001 C CNN
F 3 "" H 3700 4250 50  0000 C CNN
	1    3700 4250
	-1   0    0    1   
$EndComp
Text Notes 2850 2500 0    60   ~ 0
Backflow Protection
$Comp
L FUSE F1
U 1 1 57B32410
P 4150 2800
F 0 "F1" H 4250 2850 50  0000 C CNN
F 1 "PPTC2A" H 4050 2750 50  0000 C CNN
F 2 "Resistors_SMD:R_1812" H 4150 2800 50  0001 C CNN
F 3 "" H 4150 2800 50  0000 C CNN
	1    4150 2800
	1    0    0    -1  
$EndComp
Text GLabel 10000 5250 2    60   Input ~ 0
GND
$Comp
L Q_PMOS_GSD Q2
U 1 1 57B3250D
P 8000 3950
F 0 "Q2" V 8200 4150 50  0000 R CNN
F 1 "AO3401" V 8250 4000 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 8200 4050 50  0001 C CNN
F 3 "" H 8000 3950 50  0000 C CNN
	1    8000 3950
	0    1    -1   0   
$EndComp
$Comp
L R R6
U 1 1 57B3260E
P 7750 4350
F 0 "R6" V 7830 4350 50  0000 C CNN
F 1 "10k" V 7750 4350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 7680 4350 50  0001 C CNN
F 3 "" H 7750 4350 50  0000 C CNN
	1    7750 4350
	0    -1   -1   0   
$EndComp
$Comp
L Q_NMOS_GSD Q3
U 1 1 57B32914
P 8100 4850
F 0 "Q3" H 8400 4900 50  0000 R CNN
F 1 "2N7002" H 8750 4800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 8300 4950 50  0001 C CNN
F 3 "" H 8100 4850 50  0000 C CNN
	1    8100 4850
	-1   0    0    -1  
$EndComp
Text Notes 4900 2400 0    60   ~ 0
Supply to micro\nAlways on
Text Notes 9900 3600 2    60   ~ 0
Switched supply to PI
Text HLabel 10000 4850 2    60   Input ~ 0
PWRSWITCH
Text HLabel 10000 3850 2    60   Input ~ 0
PI5V
Text HLabel 4500 2400 2    60   Input ~ 0
5V
$Comp
L R R13
U 1 1 57B5F826
P 5250 4300
F 0 "R13" V 5330 4300 50  0000 C CNN
F 1 "10k" V 5250 4300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 5180 4300 50  0001 C CNN
F 3 "" H 5250 4300 50  0000 C CNN
	1    5250 4300
	0    -1   -1   0   
$EndComp
$Comp
L Q_NMOS_GSD Q5
U 1 1 57B5F82C
P 5600 4850
F 0 "Q5" H 5900 4900 50  0000 R CNN
F 1 "2N7002" H 6250 4800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 5800 4950 50  0001 C CNN
F 3 "" H 5600 4850 50  0000 C CNN
	1    5600 4850
	-1   0    0    -1  
$EndComp
$Comp
L Q_PMOS_GSD Q4
U 1 1 57B5F820
P 5500 3950
F 0 "Q4" V 5700 4150 50  0000 R CNN
F 1 "AO3401" V 5750 4000 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 5700 4050 50  0001 C CNN
F 3 "" H 5500 3950 50  0000 C CNN
	1    5500 3950
	0    1    -1   0   
$EndComp
Text HLabel 6400 4850 2    60   Input ~ 0
SENSSWITCH
Text HLabel 6400 3850 2    60   Input ~ 0
SENSOR5V
$Comp
L CONN_01X02 P12
U 1 1 5808AFCC
P 8150 3300
F 0 "P12" H 8150 3450 50  0000 C CNN
F 1 "CONN_01X02" V 8250 3300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 8150 3300 50  0001 C CNN
F 3 "" H 8150 3300 50  0000 C CNN
	1    8150 3300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2200 2800 3100 2800
Connection ~ 2900 2800
Connection ~ 3700 2800
Wire Wire Line
	3300 4000 3700 4000
Wire Wire Line
	3700 3800 3700 4100
Wire Wire Line
	3400 3900 3400 3800
Wire Wire Line
	2900 3900 3400 3900
Wire Wire Line
	2900 3800 2900 4100
Connection ~ 2900 3900
Wire Wire Line
	3200 3800 3200 3900
Connection ~ 3200 3900
Connection ~ 3700 4000
Wire Wire Line
	2900 4400 2900 5250
Wire Wire Line
	3700 4400 3700 5250
Connection ~ 3700 5250
Wire Wire Line
	2200 2900 2500 2900
Connection ~ 2900 5250
Wire Wire Line
	2900 2800 2900 3100
Wire Wire Line
	3700 2800 3700 3100
Wire Wire Line
	3300 3100 3300 4000
Wire Wire Line
	2500 2900 2500 5250
Wire Wire Line
	3500 2800 3900 2800
Wire Wire Line
	2500 5250 10000 5250
Wire Wire Line
	4400 2800 7800 2800
Wire Wire Line
	7900 4350 8000 4350
Connection ~ 8000 4350
Wire Wire Line
	8000 5250 8000 5050
Wire Wire Line
	8300 4850 10000 4850
Connection ~ 8000 5250
Wire Wire Line
	8200 3850 10000 3850
Connection ~ 4500 2800
Wire Wire Line
	5100 2800 5100 4300
Wire Wire Line
	5400 4300 5500 4300
Connection ~ 5500 4300
Wire Wire Line
	5500 5250 5500 5050
Connection ~ 5500 5250
Connection ~ 5100 2800
Wire Wire Line
	5500 4150 5500 4650
Wire Wire Line
	5300 3850 5100 3850
Connection ~ 5100 3850
Wire Wire Line
	5700 3850 6400 3850
Wire Wire Line
	6400 4850 5800 4850
Wire Wire Line
	4500 2800 4500 2400
Wire Wire Line
	7800 2800 7800 3850
Wire Wire Line
	7800 3800 7600 3800
Wire Wire Line
	7600 3800 7600 4350
Connection ~ 7800 3800
Wire Wire Line
	8000 4650 8000 4150
Wire Wire Line
	7800 3500 8100 3500
Connection ~ 7800 3500
Wire Wire Line
	8200 3500 8300 3500
Wire Wire Line
	8300 3500 8300 3850
Connection ~ 8300 3850
Connection ~ 5100 3500
Connection ~ 5800 3850
Wire Wire Line
	5800 3500 5800 3850
Wire Wire Line
	5700 3500 5800 3500
Wire Wire Line
	5100 3500 5600 3500
$Comp
L CONN_01X02 P11
U 1 1 5808B7C5
P 5650 3300
F 0 "P11" H 5650 3450 50  0000 C CNN
F 1 "CONN_01X02" V 5750 3300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 5650 3300 50  0001 C CNN
F 3 "" H 5650 3300 50  0000 C CNN
	1    5650 3300
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
