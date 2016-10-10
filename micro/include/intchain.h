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
 * @file intchain.h
 * Runtime interrupt sharing
 */


#ifndef _INTCHAIN_H
#define _INTCHAIN_H

/**
 * Interface for things we may delegate an IRQ call to.
 */
class IRQDelegate {
public:
	virtual ~IRQDelegate() {}

	virtual void onIRQ() =0;
};

/**
 * One node in linked list of pointers to IRQDelegate,
 * processed during an interrupt.
 */
class IntChain {
public:

	IntChain(IRQDelegate *d)
	: delegate(d), next(0)
	{}

	void install(IntChain **dest);

	inline static void execute(IntChain *p)
	{
		while(p != 0){
			p->delegate->onIRQ();
			p=p->next;
		}
	}

private:
	IRQDelegate *delegate;
	IntChain *next;
};

#if __CORTEX_M >= 3
extern IntChain *irqc_EXTI0;
extern IntChain *irqc_EXTI1;
extern IntChain *irqc_EXTI2;
extern IntChain *irqc_EXTI3;
extern IntChain *irqc_EXTI4;
extern IntChain *irqc_EXTI5_9;
extern IntChain *irqc_EXTI15_10;
#else
extern IntChain *irqc_EXTI0_1;
extern IntChain *irqc_EXTI2_3;
extern IntChain *irqc_EXTI4_15;
#endif

#endif // _INTCHAIN_H
