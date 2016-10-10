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
 *
 * Currently is just brain-dead device-test code.
 */

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "rtos.h"
#include "Timer.h"
#include "serial.h"
#include "bbuffer.h"
#include "dtostrf.h"
#include "dht22.h"
#include "hcsr04.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

char txtbuf[128];
allocated_buffer<32> serbuf;
bounded_buffer serial_rx(serbuf);
Serial serial(USART1, &serial_rx);
DHT22 dht(GPIOB, 1, EXTI0_1_IRQn, &irqc_EXTI0_1);
HCSR04 usonic(GPIOA, 3, 2, EXTI2_3_IRQn, &irqc_EXTI2_3);

int
main(int argc, char* argv[])
{
  Timer::setup();

	uint32_t count=0;

	usonic.setup();
	dht.setup();
	serial.setup(115200);

	uint64_t t0=Timer::millis()+1000;
	bool tried;

	while (true){
		Timer::msleepUntil(t0);

	    ++count;
	    tried=false;

	    usonic.startRead();
	    if((count & 31) == 0){
	    	dht.startRead();
	    	tried=true;
	    }

	    t0+=30;
	    Timer::msleepUntil(t0);

	    uint16_t range=usonic.getRange();
	    utostr(range, 10, 4, '0', &txtbuf[0]);
	    txtbuf[4]=' ';
	    txtbuf[5]='\0';
	    serial.print(&txtbuf[0]);

	    if(tried){
	    	serial.print("\r\n");
	    	if(dht.readSuccess()){
	    		int16_t temp=dht.getTemperature();
	    		int16_t rh=dht.getHumidity();

	    		dtostrx(temp, 10, 10, 4, 1, '0', &txtbuf[0]);
	    		serial.print(&txtbuf[0]);
	    		serial.print("C\r\n");
	    		dtostrx(rh, 10, 10, 4, 1, '0', &txtbuf[0]);
	    		serial.print(&txtbuf[0]);
	    		serial.print("%\r\n");
		    }
	    	else{
	    		serial.println("-");
	    	}
	    }

	    t0+=70;
	}
}

#pragma GCC diagnostic pop
