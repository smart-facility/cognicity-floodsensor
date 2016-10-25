# PCB

This printed circuit board design implements low-power measurement gathering for the cognicity flood sensor.  It conforms to the Raspberry Pi HAT specification.

## Pi HAT

### Configuration

The HAT can be configured in one of two ways
- direct, Pi-only operation: the Pi is wired to the sensors and there is no power control
- microcontroller-directed: the microcontroller makes and caches sensor observations, and is responsible for controlling power to the Pi and sensors

Three of three-pad solder-bridge footprints are used to determine whether the sensors are connected to the Pi or the STM32.  Each must have a single solder-blob connecting its centre-pad to one side, as labelled.

In Direct mode, three additional wire links are installed, to bypass power-switching components that are not loaded.

In Microcontroller mode, the power-control and microcontroller circuitry is loaded, and the wire-links are not installed.

A 24C32 I2C EEPROM is installed, which contains configuration data used by the Pi to identify the HAT and its functionality.  Programming the EEPROM requires that the "PROG" link adjacent to the EEPROM be shorted, in order to to pull up the write-enable pin.  Otherwise, the EEPROM is read-only.

### Microcontroller

The PCB hosts an STM32F030F4 or STM32L031F4 microcontroller.  The L031 supports a 32768Hz crystal for real-time clock (RTC) functionality but the F030 does not - the crystal and its capacitors not be loaded if the F030 is used.

The microcontroller is programmed and debugged using an ST-Link v2 via the single wire debug (SWD) port at the top-left of the board.  The BOOT0 pin is pulled low unless the BOOT0 link is shorted, which allows the micro to be programmed via a serial port.  A reset button is provided.

The microcontroller controls power to the sensors and Pi (status is indicated by LEDs), is directly connected to the sensors, and has a serial connection to the Pi.

Pins PA4-PA7 are not currently used and are broken out to a header, intended for use with an 8-pin IDC ribbon cable.  They can be used as GPIO, to monitor voltages (ADC), or to implement an SPI interface.

### Power Management

The power-management block contains:
- a 5V input (screw terminals)
- a PPTC resettable fuse on the input
- a low-voltage-drop reverse-polarity/reverse-current protection circuit
- 3.3V regulator to supply the microcontroller
- 3.3V regulator (input switched) to supply the DHT22
- two P-channel FETs to control power to the Pi and the sensors

The Pi receives 5V power from this board.  The board must be provided with a clean, regulated 5V supply, through the screw-terminal connector.  The Pi should not be connected to any other power source.

The power-switching is under the control of the micro.

If the board is assembled as a Direct configuration (no micro), then most of the power control circuitry is not loaded and is bypassed by wire links.  The sensors receive power from the 5V line and through the Pi's onboard 3.3V regulator.

### Sensors

The board supports:
- a DHT22 temperature/humidity sensor or other one-wire device such as DS18B20, and
- an ultrasonic ranging module, or any similar 5V-powered device with 1 input and 1 output pin

Resistive networks are supplied for pullup (one-wire) and voltage conversion (from the 5V ranging sensor).

## License

This hardware design is (C) 2016 William Brodie-Tyrrell and is licensed under the CERN OHL; see cern_ohl_v_1_2.pdf
