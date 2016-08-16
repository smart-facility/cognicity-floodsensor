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
Text GLabel 9400 4500 2    60   Input ~ 0
GND
$Comp
L Q_PMOS_GSD Q2
U 1 1 57B3250D
P 7400 2900
F 0 "Q2" V 7600 3100 50  0000 R CNN
F 1 "AO3401" V 7650 2950 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 7600 3000 50  0001 C CNN
F 3 "" H 7400 2900 50  0000 C CNN
	1    7400 2900
	0    1    -1   0   
$EndComp
$Comp
L R R6
U 1 1 57B3260E
P 7150 3300
F 0 "R6" V 7230 3300 50  0000 C CNN
F 1 "10k" V 7150 3300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 7080 3300 50  0001 C CNN
F 3 "" H 7150 3300 50  0000 C CNN
	1    7150 3300
	0    -1   -1   0   
$EndComp
$Comp
L Q_NMOS_GSD Q3
U 1 1 57B32914
P 7500 4100
F 0 "Q3" H 7800 4150 50  0000 R CNN
F 1 "2N7002" H 8150 4050 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 7700 4200 50  0001 C CNN
F 3 "" H 7500 4100 50  0000 C CNN
	1    7500 4100
	-1   0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 57B32BA6
P 7900 4250
F 0 "R7" V 7980 4250 50  0000 C CNN
F 1 "10k" V 7900 4250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 7830 4250 50  0001 C CNN
F 3 "" H 7900 4250 50  0000 C CNN
	1    7900 4250
	1    0    0    -1  
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
	3400 3800 3400 3900
Wire Wire Line
	3400 3900 2900 3900
Wire Wire Line
	2900 3800 2900 4100
Connection ~ 2900 3900
Wire Wire Line
	3200 3800 3200 3900
Connection ~ 3200 3900
Connection ~ 3700 4000
Wire Wire Line
	2900 4400 2900 4500
Wire Wire Line
	3700 4500 3700 4400
Connection ~ 3700 4500
Wire Wire Line
	2200 2900 2500 2900
Connection ~ 2900 4500
Wire Wire Line
	2900 2800 2900 3100
Wire Wire Line
	3700 2800 3700 3100
Wire Wire Line
	3300 3100 3300 4000
Wire Wire Line
	2500 2900 2500 4500
Wire Wire Line
	3500 2800 3900 2800
Wire Wire Line
	2500 4500 9400 4500
Wire Wire Line
	7000 2800 7000 3300
Wire Wire Line
	4400 2800 7200 2800
Wire Wire Line
	7300 3300 7400 3300
Wire Wire Line
	7400 3100 7400 3900
Connection ~ 7400 3300
Wire Wire Line
	7400 4500 7400 4300
Wire Wire Line
	7700 4100 9400 4100
Wire Wire Line
	7900 4500 7900 4400
Connection ~ 7400 4500
Connection ~ 7000 2800
Wire Wire Line
	7600 2800 9400 2800
Connection ~ 7900 4500
Connection ~ 5200 2800
Text Notes 5000 3350 0    60   ~ 0
Supply to micro\nAlways on
Text Notes 9300 2550 2    60   ~ 0
Switched supply to PI
Text HLabel 9400 4100 2    60   Input ~ 0
PWRSWITCH
Connection ~ 7900 4100
Wire Wire Line
	5200 2800 5200 3100
Text HLabel 9400 2800 2    60   Input ~ 0
PI5V
Text HLabel 5200 3100 2    60   Input ~ 0
5V
$EndSCHEMATC
