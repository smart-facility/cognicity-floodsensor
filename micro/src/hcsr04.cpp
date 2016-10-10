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
 * @file hcsr04.cpp
 * Device driver for ultrasonic ranging module.
 */

#include "hcsr04.h"
#include "Timer.h"

HCSR04::HCSR04(GPIO_TypeDef *g, uint8_t pt, uint8_t pe, uint8_t ir, IntChain **ch)
 : chain(this), gpio(g), trig(pt), echo(pe), irq(ir)
{
	lastUp=0;
	dt=0;
	chain.install(ch);
}

void HCSR04::setup()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	if(gpio == GPIOA){
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, echo);
	}
	else if(gpio == GPIOB){
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, echo);
	}

	uint16_t tmask=uint16_t(1<<trig);
	uint16_t emask=uint16_t(1<<echo);

	GPIO_InitTypeDef init;
	init.GPIO_Pin=tmask;
	init.GPIO_OType = GPIO_OType_PP;
	init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	init.GPIO_Mode=GPIO_Mode_OUT;
	init.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(gpio, &init);
	GPIO_ResetBits(gpio, tmask);

	init.GPIO_Pin=emask;
	init.GPIO_OType = GPIO_OType_OD;
	init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	init.GPIO_Mode=GPIO_Mode_IN;
	init.GPIO_Speed=GPIO_Speed_10MHz;
	GPIO_Init(gpio, &init);

	EXTI_InitTypeDef exti;
	exti.EXTI_Line=emask;
	exti.EXTI_Mode=EXTI_Mode_Interrupt;
	exti.EXTI_Trigger=EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd=ENABLE;
	EXTI_Init(&exti);

	NVIC_InitTypeDef nv;
	nv.NVIC_IRQChannel=EXTI2_3_IRQn;
	nv.NVIC_IRQChannelPriority=0;
	nv.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&nv);
}

void HCSR04::startRead()
{
	dt=0;
	lastUp=0;

	uint16_t mask=uint16_t(1<<trig);

	// request-pulse
	GPIO_SetBits(gpio, mask);
	Timer::usleep(15);
	GPIO_ResetBits(gpio, mask);
}

uint16_t HCSR04::getRange()
{
	// C = 340 m/s
	// T = 2 * D / C
	// D = C * T / 2
	// microns and microseconds...

	// divide by 1000 to get mm.
	return uint16_t((dt * HALF_SPEED_SOUND)/1000);
}


void HCSR04::onIRQ()
{
	uint64_t now=Timer::micros_interrupt();
	uint16_t mask=uint16_t(1<<echo);

	if(EXTI_GetFlagStatus(mask)){
		EXTI_ClearFlag(mask);
		bool rising=((GPIO_ReadInputData(gpio) & mask) == mask);

		if(rising){
			lastUp=now;
		}
		else if(lastUp != 0){
			dt=uint32_t(now-lastUp);
		}
	}
}


