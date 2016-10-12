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
 * @file Timer.h
 * Device driver for SysTick.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "cmsis_device.h"

// ----------------------------------------------------------------------------

// forward-declaration for the friend statement
extern "C" void SysTick_Handler(void);

class Timer {
public:
	typedef void (*tickfunc_t)();

	static constexpr int SYSTICK_PRIORITY=14;

	/**
	 * Reconfigure system-clock to run at a new speed, and
	 * reprogram SysTick to run at the correct rate.
	 * @param use_hse true for HSE (xtal), false for HSI (RC)
	 * @param use_pll true to enable PLL
	 * @return new system clock speed, Hz
	 */
	static uint32_t setup(bool use_hse=true, bool use_pll=true);

	/// add a handler that will be called INSIDE the tick interrupt
	/// @param tf pointer to the new handler which will be called
	/// @return the old handler, which tf much chain to
	static tickfunc_t addTickHandler(tickfunc_t tf);

	/// busy-loop pause, NO involvement of scheduler
	static void usleep(uint32_t usec);
	/// dispatch to another thread for a while
	static void msleep(uint32_t msec);
	/// dispatch to another thread for a while
	static void msleepUntil(uint64_t when);

	/// get milliseconds since boot
	static inline uint64_t millis() { return ms_elapsed; }
	/// get microseconds since boot
	static inline uint64_t micros()
	{
		asm("cpsid i");
		// disable interrupts so we get a consistent read of the two values
		uint32_t tick_orig=SysTick->VAL;
		uint64_t ms_orig=ms_elapsed;
		asm("cpsie i");

		return (ms_orig + 1) * 1000 - tick_orig/TICKS_PER_US;
	}

	/// microseconds since boot; only call this from inside an interrupt
	/// of higher priority than SysTick
	static inline uint64_t micros_interrupt()
	{
		uint32_t tick_orig=SysTick->VAL;
		uint64_t ms_orig=ms_elapsed;

		return (ms_orig + 1) * 1000 - tick_orig/TICKS_PER_US;
	}

private:
	Timer();  // no constructing!

	static uint32_t TICKS_PER_US;
	static uint32_t TICKS_PER_MS;
	static volatile uint64_t ms_elapsed;

	friend void ::SysTick_Handler(void);

	static void systemTick();
	static tickfunc_t tickfunc;
};

#endif // TIMER_H_
