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
 * @file atomic.h
 * Atomic operations for user-space code.
 */

#ifndef _ATOMIC_H
#define _ATOMIC_H

/// atomically exchange a value with a memory location
/// @param T an integer-type
/// @param p pointer to value in memory where exchange occurs
/// @param v value to install in *p
/// @return previous value of *p
template<typename T>
inline T atomicSwap(T *p, T v)
{
#if __CORTEX_M >= 3
	T result, failure;

	do {
		asm volatile (
			"ldrex %0, [%2]\n"
			"strex %1, %3, [%2]"
			: "=&r" (result), "=&r" (failure)
			: "r" (p), "r" (v)
			: "cc", "memory"
			);
	} while(failure);

	return result;
#else
	asm volatile ("cpsid i");
	T result=*p;
	*p=v;
	asm volatile ("cpsie i");
	return result;
#endif
}

/// atomically exchange a value with a memory location if it
/// contained a zero previously
/// @param T an integer-type
/// @param p pointer to value in memory where exchange occurs
/// @param v (non-zero) value to install
/// @return previous value of *p (if non-zero, then *p unchanged)
template<typename T>
inline T atomicSwapIfZero(T *p, T v)
{
#if __CORTEX_M >= 3
	T result, failure;

	do {
		asm volatile (
			"ldrex %0, [%1]\n"
			: "=&r" (result)
			: "r" (p)
			: "cc"
			);
		if(result != 0){
			return result;
		}
		asm volatile (
			"strex %0, %2, [%1]"
			: "=&r" (failure)
			: "r" (p), "r" (v)
			: "cc", "memory"
			);
	} while(failure);

	return result;
#else
	asm volatile ("cpsid i");

	T result=*p;
	if(result != 0){
		asm volatile ("cpsie i");
		return result;
	}
	*p=v;
	asm volatile ("cpsie i");
	return result;

#endif
}


/// atomically add a value to a location in memory
/// @param T an integer-type
/// @param p pointer to value in memory to modify
/// @param v delta to add to the value in memory
/// @return the new value (*p += v)
template<typename T>
inline T atomicAdd(T *p, T v)
{
#if __CORTEX_M >= 3
	T result, failure;

	do {
		asm volatile (
			"ldrex %0, [%2]\n"
			"add %0, %0, %3\n"
			"strex %1, %0, [%2]"
			: "=&r" (result), "=&r" (failure)
			: "r" (p), "r" (v)
			: "cc", "memory"
			);
	} while(failure);

	return result;
#else
	asm volatile ("cpsid i");
	T result=*p+v;
	*p=result;
	asm volatile ("cpsie i");
	return result;
#endif
}

#endif
