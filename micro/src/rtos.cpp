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
 * @file rtos.cpp
 * Threads and scheduler.
 */

#ifndef SINGLETHREAD

#include "rtos.h"
#include "Timer.h"
#include "diag/Trace.h"
#include "dtostrf.h"

#define __need_NULL
#include <stddef.h>
#include <string.h>

// light onboard LED when CPU running
#define SCHEDULER_LED_IDLE 1

#if __CORTEX_M >= 3
// C13
#define SCHEDULER_LED_PORT GPIOC
#define SCHEDULER_LED_PIN  (1<<13)
#define LED_RCC_CMD RCC_APB2PeriphClockCmd
#define SCHEDULER_LED_PERIPH RCC_APB2Periph_GPIOC
#define DEFAULT_PSR 0x21000000
#else
// A4
#define SCHEDULER_LED_PORT GPIOA
#define SCHEDULER_LED_PIN  (1<<4)
#define LED_RCC_CMD RCC_AHBPeriphClockCmd
#define SCHEDULER_LED_PERIPH RCC_AHBPeriph_GPIOA
#define GPIO_Mode_Out_PP GPIO_Mode_OUT
#define DEFAULT_PSR 0x21000000
#endif

Thread::Thread(void (*_entry)(), uint32_t *_stackspace, uint32_t _stacklen)
 : chain(NULL), flags(FLAG_RUNNABLE), stack_limit(uint32_t(_stackspace))
{
	if(_stackspace != NULL){
		sp=uint32_t(_stackspace)+_stacklen-sizeof(stackframe_t);
		stackframe_t *stack=(stackframe_t *) sp;

		// default PSR at boot
		stack->psr=DEFAULT_PSR;
		// return-to-entrypoint
		stack->pc=_entry;
		// "return" location, should never be reached
		stack->lr=(uint32_t) &Thread::thread_exit;
	}
	else{
		sp=NULL;
	}
}

void Thread::thread_exit()
{
	// some idiot returned out of _entry
	Thread *current=Scheduler::current;

	// make it not schedulable
	current->flags = (current->flags & ~FLAG_MODE) | FLAG_EXITED;

	// go into the scheduler
	yield();
}

void Thread::sleepUntil(uint64_t when)
{
	Thread *t=Scheduler::current;
	t->awaken=when;
	t->flags=(t->flags & ~FLAG_MODE) | FLAG_WAITING;
	yield();
}

void Thread::block(Condition *c)
{
	flags=(flags & ~FLAG_MODE) | FLAG_BLOCKED;
	blocker=c;
	// ENABLE_INTERRUPTS;
	// yield();
}

void idlefunc()
{
	while(true){
		asm( "nop" );
	}
}

Thread idlethread(idlefunc, NULL, 1024);

#if SCHED_RR

Thread *SchedRoundRobin::thread[SchedRoundRobin::MAXTHREADS];
Thread *SchedRoundRobin::current=NULL;
Thread *SchedRoundRobin::idle=NULL;
Thread *SchedRoundRobin::next=NULL;
int SchedRoundRobin::robin=0;
bool SchedRoundRobin::enabled=true;

void SchedRoundRobin::setup()
{
	idle=&idlethread;
	current=idle;

	for(int i=0;i<MAXTHREADS;++i){
		thread[i]=NULL;
	}

#if SCHEDULER_LED_IDLE
	  // Enable GPIO Peripheral clock
	  LED_RCC_CMD(SCHEDULER_LED_PERIPH, ENABLE);
	  GPIO_InitTypeDef GPIO_InitStructure;

	  // Configure pin in output push/pull mode
	  GPIO_InitStructure.GPIO_Pin = SCHEDULER_LED_PIN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
#if __CORTEX_M == 0
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
	  GPIO_Init(SCHEDULER_LED_PORT, &GPIO_InitStructure);
#endif

#if __CORTEX_M >= 3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
#endif

	// set priorities
	NVIC_SetPriority(PendSV_IRQn, PENDSV_PRIORITY);
	// SVC is same as SysTick so they don't preempt/reenter each other
	// and they're the only two places that schedule() gets called
#if __CORTEX_M >= 3
	NVIC_SetPriority(SVCall_IRQn, SVCALL_PRIORITY);
#else
	NVIC_SetPriority(SVC_IRQn, SVCALL_PRIORITY);
#endif
}

void SchedRoundRobin::start()
{
	// ticks be going!
	Timer::setup();
	ENABLE_INTERRUPTS;
	asm volatile ("svc 0 \n" :::);
}

void SchedRoundRobin::schedule()
{
	int oldpos=robin;
	int sparrow=robin;

	if(!enabled)
		return;

	Thread *t=NULL;

	while(true) {
		++sparrow;
		if(sparrow >= MAXTHREADS || thread[sparrow] == NULL)
			sparrow=0;

		t=thread[sparrow];

		// found a good'un
		if(t != NULL && t->runnable()){
			break;
		}

		// got back to where we started, without finding any runnable
		// => use idle-thread
		if(sparrow == oldpos){
			t=idle;
			break;
		}
	}

	// disable interrupts so we can atomically write to the scheduler shared-state
	DISABLE_INTERRUPTS;
	robin=sparrow;
	next=t;
	ENABLE_INTERRUPTS;

	// selected thread changes, make the dispatcher happen
	if(current != next){
		SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
	}
}

bool SchedRoundRobin::add(Thread *t)
{
	for(int i=0;i<MAXTHREADS;++i){
		if(thread[i] == NULL){
			thread[i]=t;
			return true;
		}
	}
	return false;
}

void SchedRoundRobin::enablePreempt(bool en)
{
	// this flag is required because a higher-priority interrupt may signal()
	// on a condition, which will call the scheduler
	enabled=en;

	if(enabled){
		// allow all interrupts
		asm volatile ("msr basepri, %0\n" :: "r"(0));

		// scheduling was disabled for a while, so get in now
		Thread::yield();
	}
	else{
		// prevent SysTick from being executed
		asm volatile ("msr basepri, %0\n" :: "r"(Timer::SYSTICK_PRIORITY));
	}
}

#endif

/// returning to this address tells exception hardware to unwind from MSP
constexpr uint32_t EXC_RETURN_MSP=0xFFFFFFF9;
/// returning to this address tells exception hardware to unwind from PSP
constexpr uint32_t EXC_RETURN_PSP=0xFFFFFFFD;

/**
 * dispatcher; tail-chained onto interrupts when a dispatch needs to occur.
 */
extern "C" void PendSV_Handler(void)
{
#if __CORTEX_M >= 3
	// save context
	// r0-r3 were already preserved
	asm volatile (
		"cpsid i\n"
		"mrs   r0, msp\n"
		"stmdb r0!, {r4-r11}\n"
		"msr   msp, r0\n"
			::: "r0", "memory"
			);
#else
	asm volatile (
		"cpsid i\n"
		"mrs	r0, msp\n"

		"sub	r0, r0, #32\n"
		"str	r7, [r0, #28]\n"
		"str	r6, [r0, #24]\n"
		"str	r5, [r0, #20]\n"
		"str	r4, [r0, #16]\n"
		"mov	r7, r11\n"
		"mov	r6, r10\n"
		"mov	r5, r9\n"
		"mov	r4, r8\n"
		"str	r7, [r0, #12]\n"
		"str	r6, [r0, #8]\n"
		"str	r5, [r0, #4]\n"
		"str	r4, [r0, #0]\n"

		"msr	msp, r0\n"
			::: "r0", "memory"
	);
#endif

	// backup the stack pointer
	asm volatile(
		"mrs   r0, msp\n"
		"str   r0, [%0]\n"
			:
			: "r"(&Scheduler::current->sp)
			: "r0", "cc", "memory"
	);

	// check for stack-overflow
	if(Scheduler::current->sp <= Scheduler::current->stack_limit){
		asm volatile ("cpsid i");
		while(1){
			;
		}
	}

	// go to the new thread; this write is the atomic point
	// at which dispatch is considered to occur
	// (doesn't matter how many times around here that schedule() runs)
	Scheduler::current=Scheduler::next;

#if SCHEDULER_LED_IDLE
	if(Scheduler::current == Scheduler::idle){
		SCHEDULER_LED_PORT->BSRR = SCHEDULER_LED_PIN;
	}
	else{
		SCHEDULER_LED_PORT->BRR = SCHEDULER_LED_PIN;
	}
#endif

	// get new stack pointer and restore context
#if __CORTEX_M >= 3
	asm volatile (
		"ldr   r0, [%0]\n"
		"ldmia r0!, {r4-r11}\n"
		"msr   msp, r0\n"
		"cpsie i\n"
			:
			: "r"(&Scheduler::current->sp)
			: "r0", "memory"
	);
#else
	asm volatile (
		"ldr	r0, [%0]\n"
		"ldmia	r0!, {r4-r7}\n"
		"mov	r11, r7\n"
		"mov	r10, r6\n"
		"mov	r9, r5\n"
		"mov	r8, r4\n"
		"ldmia	r0!, {r4-r7}\n"
		"msr	msp, r0\n"
		"cpsie	i\n"
			:
			: "r"(&Scheduler::current->sp)
			: "r0", "memory"
	);
#endif
}

#if __CORTEX_M >= 3
char hf_prints[256];
extern "C" void HardFault_Handler(void)
{
	asm volatile (
		"tst    lr, #4 \n"
		"ite    eq \n"
		"mrseq  r0, msp \n"
		"mrsne  r0, psp \n"
		"b      HardFault_show \n"
//			: "=&r"(sp)
//			::
	);
}

extern "C" void HardFault_show(uint32_t *sp)
{
	strcpy(hf_prints, "SP ");
	utostr(uint32_t(sp), 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nR0   ");
	utostr(sp[0], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nR1   ");
	utostr(sp[1], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nR2   ");
	utostr(sp[2], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nR3   ");
	utostr(sp[3], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nR12  ");
	utostr(sp[4], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nLR   ");
	utostr(sp[5], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nPC   ");
	utostr(sp[6], 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nPSR  ");
	utostr(sp[7], 16, 8, '0', hf_prints+strlen(hf_prints));

	strcat(hf_prints, "\nBFAR ");
	utostr(SCB->BFAR, 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nCFSR ");
	utostr(SCB->CFSR, 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nHFSR ");
	utostr(SCB->HFSR, 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nDFSR ");
	utostr(SCB->DFSR, 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nAFSR ");
	utostr(SCB->AFSR, 16, 8, '0', hf_prints+strlen(hf_prints));
	strcat(hf_prints, "\nSHCSR ");
	utostr(SCB->SHCSR, 16, 8, '0', hf_prints+strlen(hf_prints));
/*
	trace_puts("Hard Fault\n");
	trace_puts("SP");
	utostr(uint32_t(sp), 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nR0");
	utostr(sp[0], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nR1");
	utostr(sp[1], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nR2");
	utostr(sp[2], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nR3");
	utostr(sp[3], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nR12");
	utostr(sp[4], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nLR");
	utostr(sp[5], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nPC");
	utostr(sp[6], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("\nPSR");
	utostr(sp[7], 16, 8, '0', hf_prints);
	trace_puts(hf_prints);

	trace_puts("BFAR");
	utostr(*(volatile uint32_t *)0xE000ED38, 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("CFSR");
	utostr(*(volatile uint32_t *)0xE000ED28, 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("HFSR");
	utostr(*(volatile uint32_t *)0xE000ED2C, 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("DFSR");
	utostr(*(volatile uint32_t *)0xE000ED30, 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("AFSR");
	utostr(*(volatile uint32_t *)0xE000ED3C, 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
	trace_puts("SHCSR");
	utostr(uint32_t(SCB->SHCSR), 16, 8, '0', hf_prints);
	trace_puts(hf_prints);
*/

	while(true){
		;
	}
}

#endif

/**
 * RTOS protected-entry point.  Currently used only to implement yield() by
 * invoking scheduler and maybe dispatcher
 */
extern "C" void SVC_Handler(void)
{
	Scheduler::schedule();
}

#endif // SINGLETHREAD
