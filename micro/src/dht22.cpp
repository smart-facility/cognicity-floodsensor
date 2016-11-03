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
 * @file dht22.cpp
 * Device driver for DHT22 / AM2302 temperature/humidity sensor.
 */

#include "dht22.h"
#include "Timer.h"

DHT22::DHT22(GPIO_TypeDef *g, uint8_t p, uint8_t ir, IntChain **ch)
 : chain(this), gpio(g), pin(p), irq(ir)
{
	lastUp=0;
	result=0xFFFFFFFFFF;
	chain.install(ch);
}

void DHT22::setup()
{
	state=ST_INVALID;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	if(gpio == GPIOA){
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, pin);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	}
	else if(gpio == GPIOB){
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, pin);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	}

	uint16_t mask=uint16_t(1<<pin);

	GPIO_InitTypeDef init;
	init.GPIO_Pin=mask;
	init.GPIO_OType = GPIO_OType_OD;
	init.GPIO_PuPd = GPIO_PuPd_UP;
	init.GPIO_Mode=GPIO_Mode_OUT;
	init.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(gpio, &init);
	GPIO_SetBits(gpio, mask);

	EXTI_InitTypeDef exti;
	exti.EXTI_Line=mask;
	exti.EXTI_Mode=EXTI_Mode_Interrupt;
	exti.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd=ENABLE;
	EXTI_Init(&exti);

	NVIC_InitTypeDef nv;
	nv.NVIC_IRQChannel=irq;
	nv.NVIC_IRQChannelPriority=0;
	nv.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&nv);
}

void DHT22::startRead()
{
	result=0xFFFFFFFFFF;
	state=ST_MANUAL;
	uint16_t mask=uint16_t(1<<pin);

	// request-pulse
	GPIO_ResetBits(gpio, mask);
	Timer::msleep(2);
	GPIO_SetBits(gpio, mask);
	state=ST_STARTLO1;
}

bool DHT22::readSuccess()
{
	bool success=false;
	if(state == ST_COMPLETE){
		uint8_t sum=0;
		for(int i=1;i<5;++i){
			sum=sum+uint8_t((result >> (i<<3)) & 0xFF);
		}
		sum=sum-uint8_t(result & 0xFF);

		success=(sum == 0);
	}

	state=ST_INVALID;
	return success;
}

int16_t DHT22::getTemperature()
{
	int16_t t=int16_t((result>>8) & 0xFFFF);

	// sign+magnitude conversion
	if(t & 0x8000){
		t= - int16_t(t & 0x7FFF);
	}

	return t;
}

int16_t DHT22::getHumidity()
{
	return int16_t((result>>24) & 0x0FFF);
}

void DHT22::onIRQ()
{
	uint64_t now=Timer::micros_interrupt();
	uint16_t mask=uint16_t(1<<pin);

	if(EXTI_GetFlagStatus(mask)){
		EXTI_ClearFlag(mask);
		bool rising=((GPIO_ReadInputData(gpio) & mask) == mask);

		// nothing doing?
		// ignore race in clearing the ST_MANUAL state
		if(state <= ST_MANUAL || (state == ST_STARTLO1 && rising)){
			return;
		}

		// transition in wrong direction for the current state?
		// should never happen, implies we lost an interrupt
		bool odd=((state & 1) != 0);
		if(odd != rising){
			state=ST_INVALID;
			return;
		}

		// done?
		if(state == ST_END){
			state=ST_COMPLETE;
			return;
		}

		// calculate timing
		if(rising){
			lastUp=now;
		}
		else if(state >= ST_FIRST_BITHI && state <= ST_LAST_BITLO){
			uint32_t dt=uint32_t(now-lastUp);
			result=(result << 1) | (dt > BIT_THRESHOLD ? 1 : 0);
		}

		// proceed
		++state;
	}
}
