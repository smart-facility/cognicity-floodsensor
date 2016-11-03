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
 * @file conditions.cpp
 * Control over thread blocking, critical sections, etc.
 */

#include "conditions.h"
#include "rtos.h"
#include "atomic.h"

#include "diag/Trace.h"
#include <assert.h>

#ifndef SINGLETHREAD

void SpinLock::lock()
{
	uint32_t me=uint32_t(Scheduler::getCurrent());

	// retry-loop until we can take it
	while(true){
		uint32_t prev=atomicSwapIfZero(&spinvalue, me);

		assert(prev != me && "SpinLock re-acquisition is failure");

		// yay!
		if(prev == 0){
			asm volatile ("dmb");
			return;
		}

		Thread::yield();
	}
}

/// release the mutex
void SpinLock::unlock()
{
	asm volatile ("dmb");
	uint32_t prev=atomicSwap(&spinvalue, 0UL);
	assert(prev == uint32_t(Scheduler::getCurrent()) && "SpinLock::signal from non-holder");
	// no blocked-chain, any waiters are just polling
}


void Condition::block()
{
	Thread *t=Scheduler::getCurrent();

	t->chainTo(chain);
	chain=t;
	t->block(this);
}

Condition::~Condition()
{
	assert(chain == 0 && "Condition destroyed with threads blocked");
}


void Condition::unblock()
{
	while(chain != 0){
		Thread *nextc=chain->getChain();
		chain->wake();
		chain=nextc;
	}
}

void Mutex::wait()
{
	uint32_t me=uint32_t(Scheduler::getCurrent());

#if DUMB_LOCKING
	while(true){
		DISABLE_INTERRUPTS;
		bool ok=(value == 0);
		if(ok){
			value=me;
		}
		else{
			block();
		}
		ENABLE_INTERRUPTS;

		if(ok){
			break;
		}
		Thread::yield();
	}
	asm volatile("dmb");
#else
	// retry-loop until we can take it
	while(true){
		lock();

		assert(value != me && "Mutex re-acquisition is failure");

		// don't bother with atomicSwap as we're inside spinlock
		if(value == 0){
			value=me;
			unlock();
			return;
		}

		block();
		unlock();
		Thread::yield();
	}
#endif
}

bool Mutex::tryWait()
{
	uint32_t me=uint32_t(Scheduler::getCurrent());
	bool success=false;
#if DUMB_LOCKING
	DISABLE_INTERRUPTS;
	if(value == 0){
		success=true;
		value=me;
		asm volatile("dmb");
	}
	ENABLE_INTERRUPTS;
#else
	lock();

	assert(value != me && "Mutex re-acquisition is failure");
	if(value == 0){
		success=true;
		value=me;
	}

	unlock();
#endif
	return success;
}

void Mutex::signal()
{
#if DUMB_LOCKING
	asm volatile("dmb");
	DISABLE_INTERRUPTS;
	assert(value == uint32_t(Scheduler::getCurrent()) && "Mutex::signal from non-holder");
	value=0L;
	unblock();
	ENABLE_INTERRUPTS;
#else
	lock();
	assert(value == uint32_t(Scheduler::getCurrent()) && "Mutex::signal from non-holder");
	value=0L;
	unblock();
	unlock();
#endif
	Thread::yield();
}

#endif // SINGLETHREAD

#if !DUMB_LOCKING && !SINGLETHREAD
void Semaphore::do_unlock(bool force)
{
	int32_t intinc=0;

	while(true){
		unlock();

		// captures all calls to incFromInterrupt while the spinlock was active
		// (atomicSwap makes sure that only one thread captures the value)
		intinc=atomicSwap(&fromint, 0L);

		// an interrupt could well happen right here, and wake the whole chain
		// but that's OK, we'll either wake them all again, or have nothing left
		// to wake, depending on where we get re-scheduled.

		// need to wake other threads either if our own increment raised
		// counter above zero, or any calls were made to incFromInterrupt while we
		// had the chain locked.
		if(intinc > 0 || force){
			lock();
			value+=intinc;
			if(value > 0 || force){
				unblock();
			}
			force=false;
		}
		else{
			// no pending increments or wakes
			Thread::yield();
			return;
		}

		// this loop deals with the pathological case where incFromInterrupt() was
		// called while we held the spinlock to maybe not do the unblock()
	}
}
#endif  // dumb locking

#ifndef SINGLETHREAD
void Semaphore::inc(uint32_t qty)
{
#if DUMB_LOCKING
	DISABLE_INTERRUPTS;
	value+=qty;
	if(value > 0){
		unblock();
	}
	ENABLE_INTERRUPTS;
#else
	lock();
	value+=qty;
	int32_t newv=value;  // no need for atomicAdd because we're inside spinlock

	do_unlock(newv == 1);
#endif
}
#endif

void Semaphore::incFromInterrupt(uint32_t qty)
{
#if DUMB_LOCKING
	inc(qty);
	Scheduler::schedule();

#elif SINGLETHREAD

	DISABLE_INTERRUPTS;
	value+=qty;
	ENABLE_INTERRUPTS;

#else
	DISABLE_INTERRUPTS;  // not re-entrant, as we may need to mutate the blocking-chain

	// spinlock not held, just go ahead and increment counter
	if(spinvalue == 0){
		value+=qty;
		if(value >= 1){
			// wake the sleepers
			unblock();

			// Scheduler::schedule();
		}
	}
	else{
		// spinlock is held, write to our own private value
		++fromint;
		// and depend on the holder of the spinlock to call unblock() in the thread
	}

	ENABLE_INTERRUPTS;
#endif
}

bool Semaphore::decFromInterrupt(uint32_t qty)
{
	DISABLE_INTERRUPTS;
	if(value >= int32_t(qty)){
		value-=qty;
		ENABLE_INTERRUPTS;
		return true;
	}
	ENABLE_INTERRUPTS;
	return false;
}

void Semaphore::dec(uint32_t qty)
{
#if DUMB_LOCKING
	while(true){
		DISABLE_INTERRUPTS;
		if(value >= int32_t(qty)){
			value-=qty;
			break;
		}

		block();
		ENABLE_INTERRUPTS;

		Thread::yield();
	}
	ENABLE_INTERRUPTS;
#elif SINGLETHREAD

	while(value < qty)
		asm("nop");

	DISABLE_INTERRUPTS;
	value-=qty;
	ENABLE_INTERRUPTS;

#else
	// blocking loop, i.e. wait until value >0
	while(true){

		lock();

		// woo!
		if(value >= qty){
			break;
		}

		// process pending incFromInterrupt, might unblock us!
		block();
		do_unlock(false);
	}

	// perform the decrement
	value-=qty;
	// process any pending incFromInterrupt while the lock was held.
	do_unlock(false);
#endif
}

