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
 * @file intchain.cpp
 * Runtime interrupt sharing
 */

#include "intchain.h"
#include "cmsis_device.h"

void IntChain::install(IntChain **dest)
{
	next=*dest;
	*dest=this;
}

#if __CORTEX_M >= 3
IntChain *irqc_EXTI0=0;
IntChain *irqc_EXTI1=0;
IntChain *irqc_EXTI2=0;
IntChain *irqc_EXTI3=0;
IntChain *irqc_EXTI4=0;
IntChain *irqc_EXTI9_5=0;
IntChain *irqc_EXTI15_10=0;

extern "C" void EXTI0_IRQHandler()
{
	IntChain::execute(irqc_EXTI0);
}

extern "C" void EXTI1_IRQHandler()
{
	IntChain::execute(irqc_EXTI1);
}

extern "C" void EXTI2_IRQHandler()
{
	IntChain::execute(irqc_EXTI2);
}

extern "C" void EXTI3_IRQHandler()
{
	IntChain::execute(irqc_EXTI3);
}

extern "C" void EXTI4_IRQHandler()
{
	IntChain::execute(irqc_EXTI4);
}

extern "C" void EXTI9_5_IRQHandler()
{
	IntChain::execute(irqc_EXTI9_5);
}

extern "C" void EXTI15_10_IRQHandler()
{
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	IntChain::execute(irqc_EXTI15_10);
}
#else  // cortex M0
IntChain *irqc_EXTI0_1=0;
IntChain *irqc_EXTI2_3=0;
IntChain *irqc_EXTI4_15=0;

extern "C" void EXTI0_1_IRQHandler()
{
	IntChain::execute(irqc_EXTI0_1);
}

extern "C" void EXTI2_3_IRQHandler()
{
	IntChain::execute(irqc_EXTI2_3);
}

extern "C" void EXTI4_15_IRQHandler()
{
	IntChain::execute(irqc_EXTI4_15);
}

#endif

/// collect all other interrupts and halt
void __attribute__ ((section(".after_vectors"))) Default_Handler(void)
{
	uint32_t psr;

	asm volatile (
			"mrs %0, psr \n"
			: "=&r"(psr)
	);

	while(true){
	}
}
