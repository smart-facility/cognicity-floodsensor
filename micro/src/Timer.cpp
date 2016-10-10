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
 * @file Timer.cpp
 * Device driver for SysTick.
 */

#include "Timer.h"
#include "cortexm/ExceptionHandlers.h"
#include "rtos.h"


volatile uint64_t Timer::ms_elapsed;
uint32_t Timer::TICKS_PER_US;
uint32_t Timer::TICKS_PER_MS;
Timer::tickfunc_t Timer::tickfunc=&Timer::systemTick;

void Timer::setup()
{
	  TICKS_PER_US = SystemCoreClock / 1000000;
	  TICKS_PER_MS = TICKS_PER_US * 1000;
	  ms_elapsed=0;
	  NVIC_SetPriority(SysTick_IRQn, SYSTICK_PRIORITY);  // lowest priority except sched
	  SysTick_Config(TICKS_PER_MS);
}

void Timer::msleep(uint32_t ms)
{
#ifndef SINGLETHREAD
	Thread::sleepUntil(ms_elapsed+ms);
#else
	uint64_t when=ms_elapsed+ms;
	while(ms_elapsed < when)
		;
#endif
}

void Timer::msleepUntil(uint64_t when)
{
#ifndef SINGLETHREAD
	Thread::sleepUntil(when);
#else
	while(ms_elapsed < when)
		;
#endif
}

void Timer::usleep(uint32_t us)
{
	asm volatile ("cpsid i");
	uint32_t us_orig=SysTick->VAL / TICKS_PER_US;
	uint64_t ms_orig=ms_elapsed;
	asm volatile ("cpsie i");

	uint32_t ticks_end;
	uint64_t ms_end;
	if(us > us_orig){
		uint32_t ms_togo=int32_t(us - us_orig - 1)/1000 + 1;
		uint32_t us_togo=us - us_orig - 1000*(ms_togo - 1);

		ticks_end=(1000 - us_togo) * TICKS_PER_US;
		ms_end=ms_orig+ms_togo;
	}
	else{
		ticks_end=(us_orig-us)*TICKS_PER_US;
		ms_end=ms_orig;
	}

	while(ms_elapsed < ms_end || (SysTick->VAL > ticks_end && ms_elapsed == ms_end))
	 	asm("nop");
}

Timer::tickfunc_t Timer::addTickHandler(Timer::tickfunc_t tf)
{
	tickfunc_t oldtf=tickfunc;
	tickfunc=tf;
	return oldtf;
}

extern "C" void
SysTick_Handler(void)
{
  (*Timer::tickfunc)();
}

void Timer::systemTick(void)
{
	++ms_elapsed;
#ifndef SINGLETHREAD
	Scheduler::schedule();
#endif
}

