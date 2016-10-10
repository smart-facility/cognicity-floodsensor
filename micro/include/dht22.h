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
 * @file dht22.h
 * Device driver for DHT22 / AM2302 temperature/humidity sensor.
 */

#ifndef DHT22_H_
#define DHT22_H_

#include "intchain.h"
#include "cmsis_device.h"
#include "conditions.h"

/*
 * Driver for DHT22/AM2302 temperature/humidity sensor.
 *
 * Uses pin-change interrupt reading SysTick to measure bit timings.
 */
class DHT22 : public IRQDelegate {
public:

	/// @param g the GPIO port to use
	/// @param p the pin-number in the GPIO port
	/// @param ir the EXTI interrupt-number corresponding to the pin
	/// @param ch the chain used to install the interrupt handler
	DHT22(GPIO_TypeDef *g, uint8_t p, uint8_t ir, IntChain **ch);

	/// configure GPIO etc
	void setup();

	/// send a read-request pulse
	void startRead();

	/// find out if we got any reply
	bool readSuccess();

	/// get temperature, in 0.1C resolution
	int16_t getTemperature();
	/// get RH, in 0.1% resolution
	int16_t getHumidity();

	/// called inside IRQ, used to determine timing
	/// this must be installed via an IntChain to the appropriate EXTI
	void onIRQ();

private:

	IntChain chain;

	GPIO_TypeDef *gpio;

	/// timing of pin-changes
	uint64_t lastUp;

	/// where the bits go
	uint64_t result;

	uint16_t state;
	uint8_t pin, irq;

	// state machine; one per transition
	// state indicates what edge we're waiting for
	enum {
		ST_INVALID = 0,		///< nothing should happen
		ST_COMPLETE,		///< completed a read
		ST_MANUAL,			///< transition is due to output
		ST_STARTLO1 = 16,	///< first expected pull-down
		ST_STARTHI,
		ST_STARTLO2,
		ST_FIRST_BITHI,
		ST_FIRST_BITLO,
		ST_LAST_BITHI=ST_FIRST_BITHI+78,
		ST_LAST_BITLO,
		ST_END,				///< last expected pull-up
	};

	/// microseconds: longer than this makes it a 1.
	static constexpr unsigned BIT_THRESHOLD=50;
};


#endif /* DHT22_H_ */
