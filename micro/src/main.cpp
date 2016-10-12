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

char txtbuf[128];
FloodSensor flood;

uint32_t PERIOD=60000;
uint32_t PERDUMP=60;

int
main(int argc, char* argv[])
{
  Timer::setup(false, true);
  flood.setup();  // UART configured to work with PLL on

  uint64_t t0=Timer::millis();
  uint32_t count=0;

  flood.setSensorPower(true);

  while(true){
	  flood.getSerial().print(".");
	  flood.observe();

	  // hourly dumps
	  if((count % PERDUMP) == 0){
		  flood.getSerial().println("");
		  flood.dump();
	  }

	  // one-minute period
	  ++count;
	  t0+=PERIOD;

	  Timer::setup(false, false);   // slow the clock!
	  Timer::msleepUntil(t0);
	  Timer::setup(false, true);	// back up to speed
  }
}

#pragma GCC diagnostic pop
