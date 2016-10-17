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
 * @file flood.cpp
 * Main logic for the flood sensor control.
 */

#include "flood.h"
#include "Timer.h"
#include "sort.h"
#include "dtostrf.h"

const FloodSensor::command_t FloodSensor::COMMANDS[TOTAL_CMDS]={
		{ "OK", &FloodSensor::cmd_OK },
		{ "TIME", &FloodSensor::cmd_TIME },
		{ "DUMP", &FloodSensor::cmd_DUMP },
		{ "OBSERVE", &FloodSensor::cmd_OBSERVE },
		{ "STATUS", &FloodSensor::cmd_STATUS },
		{ "PERIOD", &FloodSensor::cmd_PERIOD },
		{ "COUNT", &FloodSensor::cmd_COUNT },
		{ "OFF", &FloodSensor::cmd_OFF },
		{ "MAXDIST", &FloodSensor::cmd_MAXDIST },
		{ "MAXTEMP", &FloodSensor::cmd_MAXTEMP },
		{ "MAXHUMID", &FloodSensor::cmd_MAXHUMID }
	};

FloodSensor::FloodSensor()
: serial_rx(serbuf),
  serial(USART1, &serial_rx),
  dht(GPIOB, DHT_PIN, EXTI0_1_IRQn, &irqc_EXTI0_1),
  usonic(GPIOA, USONIC_TRIG, USONIC_ECHO, EXTI2_3_IRQn, &irqc_EXTI2_3)
{
	stored=0;
	pi_on=sens_on=false;
	cmd_valid=0;
	period=PERIOD_DEFAULT;
	perdump=PERDUMP_DEFAULT;
	thr_dist=THRDIST_DEFAULT;
	thr_temp=THRTEMP_DEFAULT;
	thr_humid=THRHUMID_DEFAULT;
}

void FloodSensor::setup()
{
	serial.setup(BAUDRATE);
	dht.setup();
	usonic.setup();

	// assume GPIOA was powered-up because the ultrasonic sensor uses it

	// configure pins as outputs
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin=PI_POWER|SENS_POWER;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Mode=GPIO_Mode_OUT;
	gpio.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &gpio);

	setSensorPower(false);
	setPiPower(false);
}

void FloodSensor::setSensorPower(bool on)
{
	GPIO_WriteBit(GPIOA, SENS_POWER, on ? Bit_SET : Bit_RESET);
	sens_on=on;
}

void FloodSensor::setPiPower(bool on)
{
	GPIO_WriteBit(GPIOA, PI_POWER, on ? Bit_SET : Bit_RESET);
	pi_on=on;
}

void FloodSensor::observe()
{
	if(stored >= STORE_SIZE){
		return;
	}

	bool was_on=sens_on;
	if(!sens_on){
		setSensorPower(true);
		Timer::msleep(1000);	// let the DHT22 wake up
	}

	observation_t &obs=store[stored++];
	uint16_t dists[SENSOR_RETRIES];
	unsigned dcount=0;

	obs.when=Timer::millis();
	obs.distance=0;
	obs.temperature=0;
	obs.humidity=0;

	dists[0]=0;  // fallback if all distance measures fail.

	for(unsigned i=0;i<SENSOR_RETRIES;++i){
		dht.startRead();
		usonic.startRead();
		Timer::msleep(30);

		uint16_t range=usonic.getRange();
		if(range > 0){
			dists[dcount++]=range;
		}
		if(dht.readSuccess()){
			obs.temperature=dht.getTemperature();
			obs.humidity=dht.getHumidity();
		}
	}

	// DHT22 not talking, give it a couple seconds between retries
	if(obs.humidity == 0){
		for(unsigned i=0;i<SENSOR_RETRIES;++i){
			Timer::msleep(2000);
			dht.startRead();
			Timer::msleep(30);

			if(dht.readSuccess()){
				obs.temperature=dht.getTemperature();
				obs.humidity=dht.getHumidity();
				break;
			}
		}
	}

	// select median distance measurement
	selection_sort(&dists[0], dcount);
	obs.distance=dists[dcount>>1];

	setSensorPower(was_on);
}

void FloodSensor::dump()
{
	for(uint16_t i=0;i<stored;++i){
		const observation_t &obs=store[i];

		// enough for about a decade of runtime
		serial.print(utostr(obs.when, 10, 12, '0', txt));
		serial.print(",");
		serial.print(utostr(obs.distance, 10, 5, '0', txt));
		serial.print("mm,");
		serial.print(dtostrx(obs.temperature, 10, 10, 4, 1, '0', txt));
		serial.print("C,");
		serial.print(dtostrx(obs.humidity, 10, 10, 5, 1, '0', txt));
		serial.println("%");
	}
	stored=0;
}

bool FloodSensor::needUpload()
{
	// got too many?
	if(stored > 9*STORE_SIZE/10 || stored >= perdump){
		return true;
	}

	// got nothing
	if(stored == 0){
		return false;
	}

	// got too much variation?
	int16_t t_min=10000, t_max=-1000, h_min=1000, h_max=0;
	uint16_t d_min=0xFFFF, d_max=0;
	for(uint16_t i=0;i<stored;++i){
		const observation_t &obs=store[i];
		t_min=min(t_min, obs.temperature);
		t_max=max(t_max, obs.temperature);
		h_min=min(h_min, obs.humidity);
		h_max=max(h_max, obs.humidity);
		d_min=min(d_min, obs.distance);
		d_max=max(d_max, obs.distance);
	}


	return    ((t_max - t_min > thr_temp)
			|| (d_max - d_min > thr_dist)
			|| (h_max - h_min > thr_humid));
}

bool FloodSensor::haveCommand()
{
	bool got=false;

	while(serial_rx.data() > 0){

		if(cmd_valid >= command.length()-2){
			cmd_valid=0;
			serial.println("ERROR: over-long line");
		}

		// copy next byte into the command buffer
		buffer tmp(command, cmd_valid, 1);
		serial_rx.retrieve(tmp);
		char c=(char) (tmp[0]);

		switch(c){
		case '\r':
		case '\n':
			// newline => NUL-terminate and process
		{
			tmp[0]='\0';
			buffer cmd(command, 0, cmd_valid);
			processCommand(cmd);
			cmd_valid=0;
			got=true;
		}
			break;
		case 0x08:
			// backspace
			if(cmd_valid > 0){
				--cmd_valid;
			}
			break;
		default:
			// normal char; keep it
			++cmd_valid;
		}
	}

	return got;
}

void FloodSensor::processCommand(const buffer &b)
{
	if(b.length() == 0){
		return;
	}

	// super-inefficient search for matching commands,
	// but it uses no allocator and is small and obvious => good.
	for(int i=0;i<TOTAL_CMDS;++i){
		int l=strlen(COMMANDS[i].cmd_str);

		if(b.common_prefix(buffer((char *) COMMANDS[i].cmd_str)) == l){
			(this->*COMMANDS[i].cmd_func)(buffer(b, l, b.length()));
			return;
		}
	}

	serial.print("ERROR: '");
	serial.transmit(b);
	serial.println("' not recognised");
}

void FloodSensor::cmd_OK(const buffer &args)
{
}

void FloodSensor::cmd_TIME(const buffer &args)
{
	uint64_t now=Timer::millis();
	utostr(uint32_t(now & 0xFFFFFFFF), 10, 10, '0', txt);
	serial.println(txt);
}

void FloodSensor::cmd_DUMP(const buffer &args)
{
	dump();
	serial.println("OK");
}

void FloodSensor::cmd_OBSERVE(const buffer &args)
{
	observe();
	serial.println("OK");
}

void FloodSensor::cmd_STATUS(const buffer &args)
{
	serial.print("Stored ");
	serial.print(utostr(stored, 10, 3, '0', txt));
	serial.print(" of ");
	serial.print(utostr(STORE_SIZE, 10, 3, '0', txt));
	serial.println(" observations");
	cmd_PERIOD(buffer());
	cmd_COUNT(buffer());
	cmd_MAXDIST(buffer());
	cmd_MAXTEMP(buffer());
	cmd_MAXHUMID(buffer());
}

void FloodSensor::cmd_PERIOD(const buffer &args)
{
	uint64_t newp=strtoul(args, 10);

	if(args.length() >= 2 && newp >= PERIOD_MIN && newp <= PERIOD_MAX){
		period=(uint32_t) newp;
	}

	serial.print("PERIOD=");
	serial.println(utostr(period, 10, 4, '0', txt));
}

void FloodSensor::cmd_COUNT(const buffer &args)
{
	uint64_t newp=strtoul(args, 10);

	if(args.length() >= 2 && newp >= PERDUMP_MIN && newp <= PERDUMP_MAX){
		perdump=(uint16_t) newp;
	}
	serial.print("COUNT=");
	serial.println(utostr(perdump, 10, 4, '0', txt));
}

void FloodSensor::cmd_OFF(const buffer &args)
{
	setPiPower(false);
}

void FloodSensor::cmd_MAXDIST(const buffer &args)
{
	uint64_t newp=strtoul(args, 10);

	if(args.length() >= 2 && newp >= THRDIST_MIN && newp <= THRDIST_MAX){
		thr_dist=(uint16_t) newp;
	}
	serial.print("MAXDIST=");
	serial.println(utostr(thr_dist, 10, 5, '0', txt));
}

void FloodSensor::cmd_MAXTEMP(const buffer &args)
{
	uint64_t newp=10*strtoul(args, 10);

	if(args.length() >= 2 && newp >= THRTEMP_MIN && newp <= THRTEMP_MAX){
		thr_temp=(uint16_t) newp;
	}
	serial.print("MAXTEMP=");
	serial.println(dtostrx(thr_temp, 10, 10, 4, 1, '0', txt));
}

void FloodSensor::cmd_MAXHUMID(const buffer &args)
{
	uint64_t newp=10*strtoul(args, 10);

	if(args.length() >= 2 && newp >= THRHUMID_MIN && newp <= THRHUMID_MAX){
		thr_humid=(uint16_t) newp;
	}
	serial.print("MAXHUMID=");
	serial.println(dtostrx(thr_humid, 10, 10, 5, 1, '0', txt));
}
