/* -*- C++ -*- */
/*
    Copyright (C) 2015-2016 William Brodie-Tyrrell
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
 * @file main.cpp
 *
 * Main process - power management for Pi collecting flood data.
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "rtos.h"
#include "Timer.h"
#include "bbuffer.h"
#include "dtostrf.h"
#include "flood.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

char txtbuf[32];
FloodSensor flood;

/// function-pointer type to implement state-machine
typedef void (*voidfunc_t)();

/// step the state-machine
void change_state(uint8_t st);


/**
 * State-machine states.
 */
enum {
	SM_WAITING,		///< waiting for next observation, at low power
	SM_OBSERVING,	///< performing an observation, Pi is off
	SM_BOOTING,		///< waiting for Pi to come up
	SM_CONNECTED,	///< Pi is up and in communication

	SM_COUNT		///< number of states
};

/// current state
uint8_t state;

/// time of next observation
uint64_t next_obs;


/**
 * State-machine functions
 */

void enter_waiting()
{
	flood.setPiPower(false);
	flood.setSensorPower(false);
}

void enter_observing()
{
	// nothing.
}

void enter_booting()
{
	flood.setPiPower(true);
	flood.setSensorPower(true);
}

void enter_connected()
{
	flood.getSerial().println("CONNECTED");
}

void poll_waiting()
{
	Timer::setup(false, false);   // slow the clock!
	Timer::msleepUntil(next_obs);
	Timer::setup(false, true);	  // back up to speed

	change_state(SM_OBSERVING);
}

void poll_observing()
{
	flood.observe();
	next_obs+=flood.getPeriod()*1000LL;

	if(flood.needUpload()){
		change_state(SM_BOOTING);
	}
	else{
		change_state(SM_WAITING);
	}
}

void poll_booting()
{
	if(Timer::millis() >= next_obs){
		flood.observe();
		next_obs+=flood.getPeriod()*1000LL;
	}

	if(flood.haveCommand()){
		change_state(SM_CONNECTED);
	}
	else{
		flood.getSerial().println("PING");
		Timer::msleep(1000);
	}
}

void poll_connected()
{
	if(Timer::millis() >= next_obs){
		flood.observe();
		next_obs+=flood.getPeriod()*1000LL;
	}

	flood.haveCommand();
	if(!flood.getPiPower()){
		change_state(SM_WAITING);
	}
}

/// functions executed on entering a state
constexpr voidfunc_t enter[SM_COUNT]={
		enter_waiting,
		enter_observing,
		enter_booting,
		enter_connected
};
/// functions executed while polling within a state
constexpr voidfunc_t poll[SM_COUNT]={
		poll_waiting,
		poll_observing,
		poll_booting,
		poll_connected
};

void change_state(uint8_t st)
{
	(enter[st])();
	state=st;
}


int main(int argc, char* argv[])
{
  Timer::setup(false, true);

  flood.setup();  // UART configured to work with PLL on
  flood.setSensorPower(false);

  // start out doing nothing, observe a second later
  next_obs=Timer::millis()+1000;
  // try to boot the Pi, first thing
  change_state(SM_BOOTING);

  // do stuff forever
  while(true){
	  (poll[state])();
  }
}

#pragma GCC diagnostic pop
