# Microcontroller

This is some software for a microcontroller, which can be used to augment the flood sensor.

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

After powering up the Pi, the micro will send the line "PING" at 1s intervals until a line is received in reply, e.g. "OK".  Once a reply is received, the micro will send "CONNECTED".

After CONNECTED, the micro does not initiate communications; it only responds to commands from the Pi.

### Pi to Micro

The following commands are understood by the micro:
- "OK": is ignored
- "TIME": responds with the time-since-boot, in decimal milliseconds
- "DUMP": sends all cached observations and clears the cache
- "OBSERVE": makes a single observation immediately, and stores it in the cache
- "STATUS": prints out the microcontroller's status: how full the cache is, and its operating settings
- "OFF": cut off power to the Pi immediately, return to background-measurement mode
- "PERIOD xxx": sets the observation period to be xxx seconds
- "COUNT xxx": sets the number of observations to cache before powering-up the Pi
- "MAXDIST xxx": sets the distance threshold to xxx mm
- "MAXTEMP xxx": sets the temperature threshold to xxx C
- "MAXHUMID xxx": sets the humidity threshold to xxx %

When using the PERIOD, COUNT or MAX* commands, the parameter must be an integer.

Once connected, commands may be sent to the micro at any time, in any order.  When the Pi is off and the micro is in background-measurement mode, it will not respond to inputs on the serial port.

The micro does not store and recall PERIOD, COUNT or MAX* parameters through power-down.  They should be programmed in by the Pi when it is powered on.

### Change Thresholds

The micro will turn on the Pi when:
- the number of stored samples reaches the COUNT value
- the total range of cached distance measurements exceeds the MAXDIST parameter,
- the total range of cached temperature measurements exceeds the MAXTEMP parameter, or
- the total range of cached humidity measurements exceeds the MAXHUMID parameter.

## Build

This is a gnu-arm-eclipse project, created in Eclipse 4.5.0 (Mars).  It uses the gnu-none-eabi-gcc compiler, currently at version 4.9.3.

## Hardware

The target microprocessor is an STM32F030F4P6.  It is designed to be installed on a Pi HAT, mounted on top of the Raspberry Pi implementing the flood sensor.

It has the following connections:
- USART1 (PA9, PA10) is connected to the Pi's serial port
- PB0 is connected to the DHT22 temperature/humidity sensor
- PA2 and PA3 are connected to the ultrasonic ranging sensor
- PA0 and PA1 are used to control the supply of power to the sensors and Raspberry Pi
- PA4-PA7 are broken out to a header for expansion purposes

The microcontroller is expected to be programmed through the SWD port, which is available on a header.

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