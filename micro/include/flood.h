/* -*- C++ -*- */
/*
    Copyright (C) 2016 William Brodie-Tyrrell
    william@brodie-tyrrell.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

/**
 * @file flood.h
 * Manage state of the cognicity flood-sensor.
 */

#ifndef FLOOD_H_
#define FLOOD_H_

#include "serial.h"
#include "dht22.h"
#include "hcsr04.h"
#include "buffer.h"
#include "bbuffer.h"

/**
 * Abstraction over all the hardware in this system, plus
 * management of its state.
 */
class FloodSensor {
public:

	/// instantiate
	FloodSensor();

	/// configure all the hardware
	void setup();

	/// turn external sensors on/off
	void setSensorPower(bool on);

	/// turn Pi on/off
	void setPiPower(bool on);

	/// make observation and store it.  Could take a few seconds
	/// depending on sensor fickleness/failures.
	void observe();

	/// transmit all stored observations
	void dump();

	Serial &getSerial() { return serial; }

private:

	/// one observation
	typedef struct {
		uint32_t when;			///< approx seconds since boot
		uint16_t distance;		///< mm
		int16_t temperature;	///< 0.1C resolution
		int16_t humidity;		///< 0.1% resolution; relative
	} observation;

	/// speed of control-connection.  Implicitly 8-N-1.
	static constexpr uint32_t BAUDRATE=115200;

	/// keep observations in-memory until they can be transmitted
	static constexpr unsigned STORE_SIZE=100;

	/// max length of a command to receive over serial port
	static constexpr unsigned MAX_CMD=32;

	// GPIO pins
	static constexpr uint16_t PI_POWER=1<<0;	///< Pi Power, PA0
	static constexpr uint16_t SENS_POWER=1<<1;	///< Sensor Power, PA1
	static constexpr uint8_t DHT_PIN=1;			///< DHT22, PB1
	static constexpr uint8_t USONIC_TRIG=3;		///< ultrasonic trigger
	static constexpr uint8_t USONIC_ECHO=2;		///< ultrasonic echo

	// timeouts and retries and putting up with lameness
	static constexpr unsigned SENSOR_RETRIES=5;	///< max number of IO attempts to make
	static constexpr unsigned DHT_PERIOD=1000;	///< ms between read-tries

	observation store[STORE_SIZE];
	allocated_buffer<MAX_CMD> serbuf;
	bounded_buffer serial_rx;
	Serial serial;
	DHT22 dht;
	HCSR04 usonic;
	uint16_t stored;
	bool pi_on, sens_on;
};


#endif /* FLOOD_H_ */
