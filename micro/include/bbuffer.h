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
 * @file bbuffer.h
 * Bounded Buffer
 */

#ifndef BBUFFER_H_
#define BBUFFER_H_

#include "conditions.h"
#include "buffer.h"

/**
 * Classic circular bounded-buffer implementation using semaphores
 * to block reads and writes.
 * Does not support concurrent readers or concurrent writers,
 * ONLY mediates between reader and writer.
 *
 * In singlethread mode, the writer MUST be in an interrupt.
 */
class bounded_buffer {
public:

	/// create bounded-buffer spanning an allocated buffer
	bounded_buffer(buffer &b);

#ifndef SINGLETHREAD
	/// attempt to insert/append some data.  Will insert as much
	/// as possible before blocking, and repeatedly block/insert
	/// until all the data has been copied in.
	void insert(const buffer &b);
#endif

	/// as per insert(), but must use this version when calling
	/// from within an interrupt instead of user thread.  Will not block
	/// @param b the data to copy in
	/// @return the number of bytes copied
	uint32_t insertFromInterrupt(const buffer &b);
	bool insertFromInterrupt(uint8_t b);

	/// attempt to extract some data.  Will continuously extract
	/// as much as possible before blocking, and repeatedly block/retrieve
	/// until all requested data has been copied out.
	void retrieve(buffer &b);

	/// attempt to retrieve data, but do not block
	/// @param b the buffer that data is copied into
	/// @return alias to portion of b, representing the data actually copied
	buffer retrieveNonBlocking(buffer &b);

	/// get the number of bytes of free space
	uint32_t space() const
	{
#if SINGLETHREAD
		return avail;
#else
		return avail.get();
#endif
	}
	/// get the number of bytes of available data
	uint32_t data() const { return used.get(); }

private:
	buffer &buf;				///< underlying data
	uint32_t readptr, writeptr;	///< read/write offsets in buf
	Semaphore used;		///< access controls
#ifndef SINGLETHREAD
	Semaphore avail;
#else
	uint32_t avail;
#endif
};



#endif /* BBUFFER_H_ */
