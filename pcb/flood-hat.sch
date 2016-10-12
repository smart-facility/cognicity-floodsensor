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
Sheet 1 5
Title "IOT Flood Sensor HAT"
Date "2016-08-05"
Rev "a"
Comp "William Brodie-Tyrrell"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L PI2GPIO J1
U 1 1 57A482C4
P 7450 3200
F 0 "J1" H 7450 3200 39  0000 C CNN
F 1 "PI2GPIO" H 7450 3250 39  0000 C CNN
F 2 "raspi:PI2HAT" H 7450 3200 39  0001 C CNN
F 3 "" H 7450 3200 39  0000 C CNN
	1    7450 3200
	1    0    0    -1  
$EndComp
Text Label 8300 4300 0    60   ~ 0
PIDHT
$Sheet
S 5100 4800 700  400 
U 57B2FB03
F0 "OptionRom" 60
F1 "optionrom.sch" 60
F2 "SCL" I R 5800 5000 60 
F3 "SDA" I R 5800 4900 60 
F4 "PIVCC" I L 5100 4900 60 
$EndSheet
Wire Wire Line
	7100 5550 7100 5750
Wire Wire Line
	7100 5750 8150 5750
Wire Wire Line
	7800 5750 7800 5550
Wire Wire Line
	7700 5550 7700 5750
Connection ~ 7700 5750
Wire Wire Line
	7600 5550 7600 5750
Connection ~ 7600 5750
Wire Wire Line
	7500 5550 7500 5750
Connection ~ 7500 5750
Wire Wire Line
	7400 5550 7400 5750
Connection ~ 7400 5750
Wire Wire Line
	7300 5550 7300 5750
Connection ~ 7300 5750
Wire Wire Line
	7200 5550 7200 5750
Connection ~ 7200 5750
Connection ~ 7800 5750
Wire Wire Line
	5800 4900 6750 4900
Wire Wire Line
	5800 5000 6750 5000
Wire Wire Line
	8150 4300 8600 4300
Wire Wire Line
	7300 2300 7300 2600
Wire Wire Line
	7200 2600 7200 2300
Connection ~ 7200 2300
Wire Wire Line
	7600 2600 7600 2300
Connection ~ 7700 2300
$Sheet
S 2800 900  1200 800 
U 57B3119C
F0 "powercontrol" 60
F1 "powercontrol.sch" 60
F2 "PWRSWITCH" I L 2800 1100 60 
F3 "PI5V" I R 4000 1100 60 
F4 "5V" I R 4000 1500 60 
F5 "SENSSWITCH" I L 2800 1200 60 
F6 "SENSOR5V" I R 4000 1200 60 
$EndSheet
Text GLabel 8150 5750 2    60   Input ~ 0
GND
Wire Wire Line
	6600 2300 7300 2300
Text Label 8000 5750 2    60   ~ 0
GND
Wire Wire Line
	7700 2300 7700 2600
$Sheet
S 2750 6000 1150 1200
U 57B33DBD
F0 "sensors" 60
F1 "sensors.sch" 60
F2 "TRIG" I R 3900 6600 60 
F3 "ECHO" I R 3900 7000 60 
F4 "DHT" I R 3900 6200 60 
F5 "5Vsensor" I L 2750 6200 60 
$EndSheet
Wire Wire Line
	3900 6200 4400 6200
Wire Wire Line
	7600 2300 8200 2300
$Sheet
S 2800 2300 1200 1300
U 57B35E6C
F0 "Micro" 60
F1 "micro.sch" 60
F2 "PWRSWITCH" I R 4000 2500 60 
F3 "5V" I L 2800 2500 60 
F4 "TXD" I R 4000 2800 60 
F5 "RXD" I R 4000 2900 60 
F6 "VCCmicro" I R 4000 3500 60 
F7 "DHT" I R 4000 3100 60 
F8 "ECHO" I R 4000 3200 60 
F9 "TRIG" I R 4000 3300 60 
F10 "SENSSWITCH" I R 4000 2600 60 
$EndSheet
Wire Wire Line
	4000 2500 4700 2500
Wire Wire Line
	4000 1500 4200 1500
Wire Wire Line
	4200 1500 4200 2000
Wire Wire Line
	4200 2000 2550 2000
Wire Wire Line
	2550 2000 2550 2500
Wire Wire Line
	2550 2500 2800 2500
Wire Wire Line
	4000 2800 4500 2800
Wire Wire Line
	4000 2900 4500 2900
Text Label 4350 2800 2    60   ~ 0
TXD
Text Label 4350 2900 2    60   ~ 0
RXD
Wire Wire Line
	8150 3200 8700 3200
Wire Wire Line
	8150 3300 8700 3300
Text Label 8500 3200 2    60   ~ 0
RXD
Text Label 8500 3300 2    60   ~ 0
TXD
Text Label 8050 2300 2    60   ~ 0
PI5V
Wire Wire Line
	4000 1100 8200 1100
Wire Wire Line
	8200 1100 8200 2300
Wire Wire Line
	2300 1100 2800 1100
Wire Wire Line
	2750 6200 2150 6200
Text Label 2600 6200 2    60   ~ 0
5Vsensor
Text Label 7000 2300 2    60   ~ 0
PIVCC
Wire Wire Line
	5100 4900 4500 4900
Text Label 4850 4900 2    60   ~ 0
PIVCC
Text Label 4550 2500 2    60   ~ 0
PISWITCH
Text Label 2750 1100 2    60   ~ 0
PISWITCH
Text Label 6650 4300 2    60   ~ 0
PITRIG
Text Label 6650 4400 2    60   ~ 0
PIECHO
Wire Wire Line
	4000 2600 4700 2600
Text Label 4150 2600 0    60   ~ 0
SESWITCH
Wire Wire Line
	2800 1200 2300 1200
Text Label 2350 1200 0    60   ~ 0
SESWITCH
Wire Wire Line
	6200 4300 6750 4300
Wire Wire Line
	6750 4400 6200 4400
$Comp
L CONN_01X03 P1
U 1 1 57B619C9
P 4600 6200
F 0 "P1" H 4600 6400 50  0000 C CNN
F 1 "CONN_01X03" V 4700 6200 50  0000 C CNN
F 2 "smd:SMD_solder_bridge_3pin" H 4600 6200 50  0001 C CNN
F 3 "" H 4600 6200 50  0000 C CNN
	1    4600 6200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P6
U 1 1 57B62C59
P 4600 6600
F 0 "P6" H 4600 6800 50  0000 C CNN
F 1 "CONN_01X03" V 4700 6600 50  0000 C CNN
F 2 "smd:SMD_solder_bridge_3pin" H 4600 6600 50  0001 C CNN
F 3 "" H 4600 6600 50  0000 C CNN
	1    4600 6600
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P7
U 1 1 57B62DBD
P 4600 7000
F 0 "P7" H 4600 7200 50  0000 C CNN
F 1 "CONN_01X03" V 4700 7000 50  0000 C CNN
F 2 "smd:SMD_solder_bridge_3pin" H 4600 7000 50  0001 C CNN
F 3 "" H 4600 7000 50  0000 C CNN
	1    4600 7000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 6100 4000 6100
Wire Wire Line
	4400 6300 4000 6300
Wire Wire Line
	3900 6600 4400 6600
Wire Wire Line
	3900 7000 4400 7000
Wire Wire Line
	4000 6500 4400 6500
Wire Wire Line
	4000 6700 4400 6700
Wire Wire Line
	4000 6900 4400 6900
Wire Wire Line
	4000 7100 4400 7100
Wire Wire Line
	4000 3100 4700 3100
Wire Wire Line
	4700 3200 4000 3200
Wire Wire Line
	4700 3300 4000 3300
Text Label 4150 3100 0    60   ~ 0
UCDHT
Text Label 4150 3200 0    60   ~ 0
UCECHO
Text Label 4150 3300 0    60   ~ 0
UCTRIG
Text Label 4100 6100 0    60   ~ 0
PIDHT
Text Label 4100 6300 0    60   ~ 0
UCDHT
Text Label 4100 6500 0    60   ~ 0
PITRIG
Text Label 4100 6700 0    60   ~ 0
UCTRIG
Text Label 4100 6900 0    60   ~ 0
PIECHO
Text Label 4100 7100 0    60   ~ 0
UCECHO
Wire Wire Line
	4000 1200 4800 1200
Text Label 4150 1200 0    60   ~ 0
5Vsensor
Text Notes 4900 6650 0    60   ~ 0
Solder Bridges
$EndSCHEMATC