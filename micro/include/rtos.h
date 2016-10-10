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
 * @file rtos.h
 * Threads and scheduler.
 */


#ifndef RTOS_H_
#define RTOS_H_

#define DISABLE_INTERRUPTS asm volatile ("cpsid i")
#define ENABLE_INTERRUPTS asm volatile ("cpsie i")

#ifndef SINGLETHREAD

#include "cmsis_device.h"
#include "Timer.h"

class SchedRoundRobin;
class Condition;
extern "C" void PendSV_Handler(void);
extern "C" void SVCall_Handler(void);

/**
 * Thread Control Block.  Each should be statically declared.
 */
class Thread {
public:

	/**
	 * create a thread; does NOT add it to the scheduler
	 * @param entry the thread's entry-point
	 * @param _stackspace pointer to the bottom of the region of memory to be used for stacking
	 * @param _stacklen the size of the stack allocation, in bytes
	 */
	Thread(void (*_entry)(), uint32_t *_stackspace, uint32_t _stacklen);

	inline bool runnable() const
	{
		uint32_t mode=flags & FLAG_MODE;
		return (mode == FLAG_RUNNABLE)
			|| (mode == FLAG_WAITING && Timer::millis() >= awaken);
	}

	/**
	 * Yield control of the CPU and pick a (probably-different)
	 * runnable thread if any available
	 */
	inline static void yield()
	{
		asm volatile ( "svc 0 \n" );
	}

	/**
	 * Put current thread out until a specified time
	 * @param when wakeup time, wrt Timer::millis()
	 */
	static void sleepUntil(uint64_t when);

	/**
	 * overwrite the chain-pointer, to insert this thread
	 * into a chain
	 */
	inline void chainTo(Thread *t)
	{
		chain=t;
	}

	/**
	 * Get the chain-pointer, and erase it.
	 */
	inline Thread *getChain()
	{
		Thread *ch=chain;
		chain=(Thread *) 0;
		return ch;
	}

	/**
	 * make a thread explicitly runnable, e.g. because a
	 * blocking mutex has signalled
	 */
	inline void wake()
	{
		blocker=0;
		flags=(flags & ~FLAG_MODE) | FLAG_RUNNABLE;
	}

	/**
	 * block a specific thread (make it not runnable)
	 * DOESN'T YIELD, that must happen after
	 * // MUST BE CALLED WITH INTERRUPTS DISABLED
	 */
	void block(Condition *c);

private:

	static void thread_exit();

	/**
	 * a saved stack-frame during a context switch
	 */
	typedef struct {
		uint32_t regs[13]; // r4-r11, r0-r3, r12,
		uint32_t lr;
		void (*pc)(void);
		uint32_t psr;
	} stackframe_t;

	/// mask on thread-state bits
	static constexpr uint32_t FLAG_MODE=0x03;
	/// waiting on timer
	static constexpr uint32_t FLAG_WAITING=0x00;
	/// ready to go!
	static constexpr uint32_t FLAG_RUNNABLE=0x01;
	/// waiting on a condition object
	static constexpr uint32_t FLAG_BLOCKED=0x02;
	/// all done, never schedule again
	static constexpr uint32_t FLAG_EXITED=0x03;

	/// chain of other threads to apply an operation (e.g. awaken) to
	Thread *chain;

	/// what we're waiting on (mostly for debug purposes)
	Condition *blocker;

	/// when to awaken if in WAITING mode
	uint64_t awaken;
	/// thread-state
	uint32_t flags;
	/// saved stack-pointer
	uint32_t sp;
	/// bottom of stack
	uint32_t stack_limit;

	friend class SchedRoundRobin;
	friend void ::PendSV_Handler(void);
};

#if SCHED_RR

class Timer;

class SchedRoundRobin {

	static constexpr int PENDSV_PRIORITY=15;
	static constexpr int SVCALL_PRIORITY=15;
	static constexpr int SYSTICK_PRIORITY=14;
	static constexpr int MAXTHREADS=16;

public:

	/// initialise RTOS state, create idle thread, etc
	static void setup();
	/// end the main (current) thread, begin threaded operation
	static void start();
	/// run the scheduler to determine next thread to run
	static void schedule();
	/// add a thread
	static bool add(Thread *t);

	/// turn pre-emption on/off temporarily, e.g. so that
	/// time-critical bit-banging will work
	static void enablePreempt(bool en);

	/// who is executing!?
	static inline Thread *getCurrent() { return current; }

private:
	SchedRoundRobin();  // no creation!

	static Thread *current;  ///< who's running when entering dispatch
	static Thread *next;	 ///< who dispatch should run
	static Thread *idle;
	static Thread *thread[MAXTHREADS];
	static int robin;		 ///< which one we're up to in the cycle
	static bool enabled;

	friend class Thread;
	friend class Timer;
	friend void ::PendSV_Handler(void);
	friend void ::SVCall_Handler(void);
};

typedef SchedRoundRobin Scheduler;

#endif // SCHED_RR

#endif // SINGLETHREAD

#endif /* RTOS_H_ */
