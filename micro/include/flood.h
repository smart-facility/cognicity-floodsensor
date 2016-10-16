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

	/// determine if Pi is on
	bool getPiPower() const { return pi_on; }

	/// make observation and store it.  Could take a few seconds
	/// depending on sensor fickleness/failures.
	void observe();

	/// transmit all stored observations
	void dump();

	Serial &getSerial() { return serial; }

	/// how many are stashed
	uint16_t getStoredCount() const { return stored; }

	/// do we need to upload values, e.g. because one has changed too much?
	bool needUpload();

	/// process serial input
	/// @return whether a complete line was received
	bool haveCommand();

	uint32_t getPeriod() const { return period; }
	uint32_t getPerDump() const { return perdump; }

private:

	/// got a complete command; parse and deal with it.
	void processCommand(const buffer &b);


	/// one observation
	typedef struct {
		uint32_t when;			///< approx seconds since boot
		uint16_t distance;		///< mm
		int16_t temperature;	///< 0.1C resolution
		int16_t humidity;		///< 0.1% resolution; relative
	} observation_t;

	void cmd_OK(const buffer &);
	void cmd_TIME(const buffer &);
	void cmd_DUMP(const buffer &);
	void cmd_OBSERVE(const buffer &);
	void cmd_STATUS(const buffer &);
	void cmd_PERIOD(const buffer &);
	void cmd_COUNT(const buffer &);
	void cmd_OFF(const buffer &);

	/// member-function-pointer for handling serial commands
	typedef void (FloodSensor::* cmd_ptr_t)(const buffer &);

	/// command-name/function pairing
	typedef struct {
		const char *cmd_str;
		cmd_ptr_t cmd_func;
	} command_t;

	/// serial-commands we know about
	enum {
		CMD_OK,			///< "OK", ignored
		CMD_TIME,		///< "TIME", send current time in ms
		CMD_DUMP,		///< "DUMP", send all stored observations
		CMD_OBSERVE,	///< "OBSERVE", make a new observation
		CMD_STATUS,		///< "STATUS", print status
		CMD_PERIOD,		///< "PERIOD xxx", set observation period, seconds
		CMD_COUNT,		///< "COUNT xxx", set reboot period, observations
		CMD_OFF,		///< "OFF", cut power to the Pi

		TOTAL_CMDS
	};

	/// keep observations in-memory until they can be transmitted
	static constexpr unsigned STORE_SIZE=200;

	/// command-name/function pairings
	static const command_t COMMANDS[TOTAL_CMDS];

	/// speed of control-connection.  Implicitly 8-N-1.
	static constexpr uint32_t BAUDRATE=115200;

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
	static constexpr uint32_t PERIOD_DEFAULT=60;
	static constexpr uint32_t PERIOD_MIN=5;
	static constexpr uint32_t PERIOD_MAX=7200;
	static constexpr uint32_t PERDUMP_DEFAULT=60;
	static constexpr uint32_t PERDUMP_MIN=1;
	static constexpr uint32_t PERDUMP_MAX=STORE_SIZE;

	observation_t store[STORE_SIZE];
	allocated_buffer<MAX_CMD> serbuf;			///< circular buffer for serial driver
	bounded_buffer serial_rx;
	allocated_buffer<MAX_CMD> command;			///< received-command buffer
	uint32_t period, perdump;
	uint16_t cmd_valid;
	Serial serial;
	DHT22 dht;
	HCSR04 usonic;
	uint16_t stored;
	char txt[16];
	bool pi_on, sens_on;
};


#endif /* FLOOD_H_ */
