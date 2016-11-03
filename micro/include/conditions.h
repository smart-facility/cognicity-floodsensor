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
 * @file conditions.h
 * Control over thread blocking, critical sections, etc.
 */

#ifndef _CONDITIONS_H
#define _CONDITIONS_H

#include "cmsis_device.h"

#ifndef SINGLETHREAD

class Thread;

// don't try to be smart with ldrex/strex, just turn interrupts off.
#define DUMB_LOCKING 1

/**
 * Spin lock which yields in each polling cycle, used to implement
 * a rapidly-retrying mutex.
 * It'd be inefficient for general-purpose use, but is suitable
 * to control a tiny section of code, e.g. a Semaphore's internal
 * state and blocking-chain.
 * This does not disable interrupts at any point and uses no
 * privileged instructions, just ldrex/strex.  Does not interact
 * with the scheduler, except to yield.
 * MUST NOT BE USED INSIDE INTERRUPT!
 */
class SpinLock {
public:

	SpinLock() : spinvalue(0) {}

	/// poll for the mutex
	void lock();

	/// release the mutex
	void unlock();

protected:
	uint32_t spinvalue;
};

/**
 * Base class for objects that Threads can block on; has
 * mechanisms for adding a thread to its block-queue and waking
 * the whole blocked-chain.
 * The blocked-chain must be manipulated ONLY from within a thread
 * and all accesses must be wrapped in SpinLock to
 * protect the chain state's consistency with whatever lock condition
 * this represents.
 */
class Condition
#if !DUMB_LOCKING
		: protected SpinLock
#endif
	{
public:

	Condition() : chain((Thread *) 0) {}
	~Condition();

protected:

	/// for unconditionally blocking;
	/// call SpinLock::lock (or disable interrupts) before calling
	/// evaluating condition that lead to block being called.
	/// should call unlock/yield after this
	/// DO NOT CALL FROM INTERRUPT
	void block();

	/// wake all waiters & remove them from the chain;
	/// call SpinLock::lock before evaluating condition that lead
	/// to this being called,
	/// then typically call unlock(), yield() after this.
	/// call from an interrupt acceptable only if spinvalue==0
	void unblock();

private:
	Thread *chain;
};

/**
 * Mutual exclusion.
 *
 * Usable only from threads, interrupts are never disabled and
 * must never interact with this.
 */
class Mutex : private Condition {
public:

	Mutex() : value(0) {}

	/**
	 * block until no one holds the mutex, then obtain it
	 */
	void wait();

	/**
	 * attempt to obtain mutex, non-blocking
	 * @return true on success, false if held by another thread
	 */
	bool tryWait();

	/**
	 * release the mutex
	 */
	void signal();

private:
	uint32_t value;
};

#endif // SINGLETHREAD

/**
 * Signed counting semaphore.
 *
 * Intended for use primarily from threads, but contains a
 * special path to allow an interrupt to inc().  Interrupts are disabled
 * ONLY inside incFromInterrupt.
 *
 * If compiled in SINGLETHREAD mode, dec() can be woken only by an
 * interrupt calling incFromInterrupt() - because it cannot yield.
 */
#ifdef SINGLETHREAD
class Semaphore {
#else
class Semaphore : private Condition {
#endif
public:
	Semaphore(int32_t init=0)
	 : value(init)
#if !DUMB_LOCKING && !SINGLETHREAD
	, fromint(0)
#endif
	{}

#ifndef SINGLETHREAD
	/**
	 * Unconditionally increment the counter and if there are
	 * other threads blocked, unblock them.
	 *
	 * Must be called from a Thread, not an interrupt.
	 *
	 * Pointless in singlethread mode
	 */
	void inc(uint32_t qty=1);
#endif

	/**
	 * same as inc() except this is the call-from-interrupt version.
	 */
	void incFromInterrupt(uint32_t qty=1);

	/**
	 * if the counter is above zero, decrement it atomically
	 * otherwise block the thread.
	 * Must not be called from an interrupt!
	 */
	void dec(uint32_t qty=1);

	/**
	 * if the counter is high enough, decrement it atomically
	 * otherwise do nothing.
	 * may be called from an interrupt
	 * @return true if the counter can be decremented.
	 */
	bool decFromInterrupt(uint32_t qty=1);

	/**
	 * obtain the semaphore counter value
	 */
	inline int32_t get() const { return value; }

private:
	/// counter value.
	volatile int32_t value;

#if !DUMB_LOCKING && !SINGLETHREAD

#error non-dumb locking is broken at the moment.

	/// release the spinlock and deal with any accumulated
	/// value in fromint, i.e. deferred incs from interrupts.
	void do_unlock(bool force);

	/// secondary counter for use by interrupts when the spinlock
	/// is held by a thread
	int32_t fromint;
#endif
};

#endif	// _CONDITIONS_H
