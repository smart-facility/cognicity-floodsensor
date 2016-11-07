# Microcontroller

This is some software for a microcontroller (STM32F030F4P6), which can be used to augment the flood sensor.

The concept of operations intended here is:
- while on, the Pi communicates with this microcontroller over a serial port, and configures it
- this microcontroller makes periodic observations of distance, temperature and humidity,
- observations with Pi turned off use very little power,
- the microcontroller periodically turns on the Pi to upload cached observations,
- once the cached observations are uploaded, the Pi requests that its power be turned off

The microcontroller continues to make observations, whether the Pi is powered-up and connected or not.

## Interface

All communications between the microcontroller and Pi are over an asynchronous serial port, configured as 115200 baud, 8 data bits, no parity bit, 1 stop bit ("8N1").

The protocol is in human-readable ASCII, and is line-oriented.  All commands must be terminated with a newline and/or carriage-return (0x0A, 0x0D).

When power is first applied to the micro, it will immediately attempt to power-up the Pi.

### Micro to Pi

After powering up the Pi, the micro will send the line `PING` at 1s intervals until a line is received in reply, e.g. `OK`.  Once a reply is received, the micro will send `CONNECTED`.

After `CONNECTED`, the micro does not initiate communications; it only responds to commands from the Pi.

### Pi to Micro

The following commands are understood by the micro:
- `OK` is ignored
- `TIME` responds with the time-since-boot, in decimal milliseconds
- `DUMP` sends all cached observations and clears the cache
- `OBSERVE` makes a single observation immediately, and stores it in the cache
- `STATUS` prints out the microcontroller's status: how full the cache is, and its operating settings
- `OFF` cut off power to the Pi immediately, return to background-measurement mode
- `PERIOD xxx` sets the observation period to be xxx seconds
- `COUNT xxx` sets the number of observations to cache before powering-up the Pi
- `MAXDIST xxx` sets the distance threshold to xxx mm
- `MAXTEMP xxx` sets the temperature threshold to xxx C
- `MAXHUMID xxx` sets the humidity threshold to xxx %

When using the `PERIOD`, `COUNT` or `MAX*` commands, the parameter must be an integer.

Once connected, commands may be sent to the micro at any time, in any order.  When the Pi is off and the micro is in background-measurement mode, it will not respond to inputs on the serial port.

The micro does not store and recall `PERIOD`, `COUNT` or `MAX*` parameters through power-down.  They should be programmed in by the Pi when it is powered on.

Commands are case-sensitive.  The micro will print an error if it does not recognise a command.  `PERIOD`, `COUNT` and `MAX*` commands with parameters missing or out of range will result in no change to those values.

### Dump Format

The `DUMP` commands prints out cached observations, one per line, followed by `OK` once all cached observations have been sent.

The format of each line is, printf-style, `"%12d,%5dmm,%2.1fC,%3.1f%%", time, distance, temperature, humidity`

The units are milliseconds, millimeters, degrees C and % (relative), respectively.

### Change Thresholds

The micro will turn on the Pi when:
- the number of stored samples reaches the `COUNT` value
- the total range of cached distance measurements exceeds the `MAXDIST` parameter,
- the total range of cached temperature measurements exceeds the `MAXTEMP` parameter, or
- the total range of cached humidity measurements exceeds the `MAXHUMID` parameter.

### Example Session

A typical session between the micro and Pi is illustrated below (with local-echo on, so that text from both the Pi and micro are intermingled).

```
PING
PING
OK
CONNECTED
STATUS
Stored 032 of 180 observations
PERIOD=0060
COUNT=0060
MAXDIST=00030
MAXTEMP=03.0
MAXHUMID=003.0
DUMP
000000001000,00108mm,22.1C,050.8%
000000062000,00097mm,22.1C,050.8%
000000122000,00097mm,22.1C,050.7%
000000182000,00101mm,22.2C,050.6%
000000242000,00101mm,22.2C,050.5%
000000302000,00101mm,22.2C,050.5%
000000362000,00101mm,22.2C,050.3%
000000422000,00099mm,22.2C,050.3%
000000482000,00099mm,22.2C,050.1%
000000542000,00103mm,22.2C,050.1%
000000602000,00086mm,22.2C,050.2%
000000662000,00085mm,22.2C,050.0%
000000722000,00080mm,22.2C,050.1%
000000782000,00084mm,22.1C,050.2%
000000842000,00084mm,22.1C,050.1%
000000902000,00084mm,22.0C,050.4%
000000962000,00084mm,21.9C,050.5%
000001022000,00084mm,22.0C,050.4%
000001082000,00084mm,21.9C,050.5%
000001142000,00084mm,21.9C,050.5%
000001202000,00084mm,22.0C,050.4%
000001262000,00084mm,22.0C,050.4%
000001322000,00084mm,22.0C,050.1%
000001382000,00084mm,22.1C,049.9%
000001442000,00083mm,22.1C,049.9%
000001502000,00086mm,22.1C,050.2%
000001562000,00088mm,22.1C,050.4%
000001622000,00081mm,22.0C,050.3%
000001682000,00081mm,22.0C,050.2%
000001742000,00092mm,22.0C,050.1%
000001802000,00092mm,22.0C,050.1%
000001862000,01632mm,22.0C,050.1%
OK
TIME
000001897169
MAXTEMP 5
MAXTEMP=05.0
OFF
```

The Pi was woken because the distance measurement suddenly diverged.  The Pi makes the following requests:
- query the micro's status
- download all cached observations
- request the current time
- set the temperature-change threshold to 5 degrees, and
- turns itself off

### Time Synchronisation

Time within the micro is derived from an internal RC oscillator which has approximately 1% accuracy.  While the timebase is relatively stable over the course of an hour or two, it will not match realtime in rate (1% error) or epoch (zero is when it was powered-on).

The Pi must therefore use linear interpolation to convert the micro's stated observation times to UTC.  Every time the Pi requests a `DUMP`, it should also request the `TIME` from the micro, and store the reported time along with an accurate (e.g. from NTP) global time.

Using the micro-time/NTP-time pairs obtained at each `DUMP`, a linear interpolation is performed to convert the observation times to a common (NTP-derived) timebase.

The Pi may call `DUMP` as soon as it is powered-on but it should wait until it is synchronised with NTP before requesting `TIME` from the micro.

## Build

This is a gnu-arm-eclipse project directory, created using Eclipse 4.5.0 (Mars) and arm-none-eabi-gcc version 4.9.3.  Later versions should be OK.

You should compile the Release target; the Debug target does not currently fit in the F4 device though it will fit in an F6 if you modify `ldscripts/mem.ld` to indicate the presence of 32kB of flash.

If you do not have the compiler suite and don't need to modify anything, it is possible to use the included `Release/micro.bin` to program a device (below).

### Download

The code is flashed into the micro using a combination of OpenOCD 0.8.0 and an ST-Link v2 USB-dongle.  Once compilation is complete, perform the following steps:
- connect the ST-Link to the target micro (GND, 3.3V, SWCLK and SWDIO)
- connect the ST-Link to a host USB port
- in one terminal window
  - `openocd -f interface/stlink-v2.cfg -f target/stm32f0x_stlink.cfg`
- in a second terminal window, while the first is still running
  - get into the `micro` directory
  - `telnet localhost 4444`
  - `reset halt`
  - `flash write_image erase Release/micro.bin 0x08000000`
  - `reset`

That will download the Release binary to the micro and start it running.  The ST-Link may now be removed and the micro deployed.

It is also possible to use gdb to debug the code, except that the (complete) software image is too large when compiled with debugging enabled (see above).  To debug:
- connect the ST-Link and start openocd in its own terminal as above
- in a second terminal window
  - get into the 'micro` directory
  - `arm-none-eabi-gdb Debug/micro.elf`
  - `target remote :3333`
  - `mon reset halt`
  - `load`
  - create breakpoints as necessary
  - `cont`

## Hardware

The target microprocessor is an STM32F030F4P6.  It is designed to be installed on a Pi HAT, mounted on top of the Raspberry Pi implementing the flood sensor.

It has the following connections:
- USART1 (PA9, PA10) is connected to the Pi's serial port
- PB1 is connected to the DHT22 temperature/humidity sensor
- PA2 (echo) and PA3 (trigger) are connected to the ultrasonic ranging sensor
- PA0 and PA1 are used to control the supply of power to the sensors and Raspberry Pi
- PA4-PA7 are broken out to a header for future-expansion purposes

The microcontroller is expected to be programmed through the SWD port, which is available on a header.

No external crystal is used; those pins are reserved on the PCB for a low-speed external (LSE; 32768Hz) oscillator in anticipation of possibly changing to an STM32L031F4 in future.  The F030 does not contain LSE hardware.

### Power Consumption

There are three relevant modes:
- idle: under 5mA,
- observe: approx 40mA, depending on sensor power consumption, and
- Pi On: approx 200-1000mA, depending on power consumption by the Pi and its radio-modem.

An observation takes, typically, approximately 1s.  It can take up to 10s if the DHT22 is failing to respond.

A typical configuration would have the micro making one observation per minute and transferring observations to the Pi once every two hours, which would take 1 minute each time.

Average current consumption (from the 5V rail) under those typical conditions will be approximately:
`Iavg = 1/120*800mA + (119/120)*( (1/60)*40mA + 59/60*5mA ) ~= 12.5mA = 62.5mW`

The system should therefore run for about a week on a 5V 2Ah supply (2 of 18650 LiPo cells), and indefinitely if a small (1W) solar panel is used to keep the battery full.

## License

Code in the `src/` and `include/` directories is licensed under the GPL version 3, copyright William Brodie-Tyrrell.

Code in the `system/` directory is provided by ST Microelectronics and is licensed under the ST Liberty Software License version 2: http://www.st.com/software_license_agreement_liberty_v2
