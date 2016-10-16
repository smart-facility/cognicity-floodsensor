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
		{ "OFF", &FloodSensor::cmd_OFF }
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

	obs.when=uint32_t(Timer::millis()/1000);
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
	char txtbuf[16];

	for(uint16_t i=0;i<stored;++i){
		const observation_t &obs=store[i];

		serial.print(itostr(obs.when, 10, 10, '0', txtbuf));
		serial.print(",");
		serial.print(utostr(obs.distance, 10, 5, '0', txtbuf));
		serial.print("mm,");
		serial.print(dtostrx(obs.temperature, 10, 10, 4, 1, '0', txtbuf));
		serial.print("C,");
		serial.print(dtostrx(obs.humidity, 10, 10, 4, 1, '0', txtbuf));
		serial.println("%");
	}
	stored=0;
}

bool FloodSensor::needUpload()
{
	return stored > 9*STORE_SIZE/10 || stored >= perdump;
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
	// but it doesn't matter.
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
	utostr(stored, 10, 3, '0', txt);
	serial.print(txt);
	serial.print(" of ");
	utostr(STORE_SIZE, 10, 3, '0', txt);
	serial.print(txt);
	serial.println(" observations");
	serial.print("Period is ");
	utostr(period, 10, 4, '0', txt);
	serial.print(txt);
	serial.print(", ");
	utostr(perdump, 10, 3, '0', txt);
	serial.print(txt);
	serial.println(" observations/reboot");
}

void FloodSensor::cmd_PERIOD(const buffer &args)
{
	uint64_t newp=strtoul(args, 10);

	if(args.length() >= 2 && newp >= PERIOD_MIN && newp <= PERIOD_MAX){
		period=newp;
	}
	serial.print("PERIOD is ");
	utostr(period, 10, 4, '0', txt);
	serial.print(txt);
	serial.println(" s");
}

void FloodSensor::cmd_COUNT(const buffer &args)
{
	uint64_t newp=strtoul(args, 10);

	if(args.length() >= 2 && newp >= PERDUMP_MIN && newp <= PERDUMP_MAX){
		perdump=newp;
	}
	serial.print("COUNT is ");
	utostr(perdump, 10, 4, '0', txt);
	serial.print(txt);
	serial.println(" observations/reboot");
}

void FloodSensor::cmd_OFF(const buffer &args)
{
	setPiPower(false);
}
