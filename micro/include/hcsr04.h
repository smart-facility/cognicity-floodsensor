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
 * @file hcsr04.h
 * Device driver for ultrasonic ranging module.
 */

#include "intchain.h"
#include "cmsis_device.h"
#include "conditions.h"

/*
 * Driver for HC-SR04 ultrasonic sensor
 *
 * Uses pin-change interrupt with SysTick to measure bit timings.
 */
class HCSR04 : public IRQDelegate {
public:

	/// @param g the GPIO port to use
	/// @param ptrig the pin-number in the GPIO port to trigger the sensor
	/// @param pecho the pin-number that carries the echo signal
	/// @param ir the EXTI interrupt-number corresponding to the pin
	/// @param ch the chain used to install the interrupt handler
	HCSR04(GPIO_TypeDef *g, uint8_t ptrig, uint8_t pecho, uint8_t ir, IntChain **ch);

	/// configure GPIO etc
	void setup();

	/// initiate a ranging
	void startRead();

	/// get distance, 1mm resolution.  0 for failure.
	uint16_t getRange();

	/// called inside IRQ, used to determine timing
	/// this must be installed via an IntChain to the appropriate EXTI
	void onIRQ();

private:

	/// m/s
	constexpr static uint32_t HALF_SPEED_SOUND=170;

	/// timing of pin-changes
	uint32_t dt;
	uint64_t lastUp;

	IntChain chain;

	GPIO_TypeDef *gpio;

	uint8_t trig, echo;
	uint8_t irq;
};

