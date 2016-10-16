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
 * @file buffer.h
 * Bounds-checked memory accessors.
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <string.h>
#include <cmsis_device.h>

template<typename T>
inline T min(const T &a, const T &b)
{
	return (b < a) ? b : a;
}

template<typename T>
inline T max(const T &a, const T &b)
{
	return (b < a) ? a : b;
}

/**
 * Representation of a finite, contiguous chunk of memory.
 *
 * This class is generally not considered to own the referred-to memory,
 * it is merely a slightly smarter/safer pointer that provides
 * some non-overflowing operations over the memory.
 */
class buffer {
public:

	/// empty buffer
	buffer()
		: ptr((uint8_t *) 0), len(0UL)
	{}

	/// share the buffer
	buffer(const buffer &o)
		: ptr(o.ptr), len(o.len)
	{
	}

	/// simple explicit construction
	/// @param p pointer to data
	/// @param l number of bytes available to p
	buffer(uint8_t *p, uint32_t l)
		: ptr(p), len(l)
	{}

	/// make a reference to an internal portion of another buffer.
	buffer(const buffer &o, uint32_t off, uint32_t sublen)
	{
		assign(o, off, sublen);
	}

	/// make a buffer from a NUL-terminated string (not incl NUL)
	explicit buffer(char *p)
		: ptr((uint8_t *) p), len(strlen(p))
	{}

	/// make a reference to an internal portion of another buffer.
	/// ensures that it doesn't go outside bounds of the source.
	void assign(const buffer &o, uint32_t off =0UL, uint32_t sublen=0xFFFFFFFF);

	inline explicit operator uint8_t *() { return ptr; }
	inline explicit operator const uint8_t *() const { return ptr; }
	inline uint32_t length() const { return len; }
	inline uint8_t& operator[](uint32_t i) { if(i >= len) return canary; return ptr[i]; }
	inline const uint8_t& operator[](uint32_t i) const { if(i >= len) return canary; return ptr[i]; }


	/// Copy from another buffer without overflowing this one.
	/// @param src source buffer
	/// @param maxlen maximum number of bytes to copy
	/// @return number of bytes copied
	uint32_t copy(const buffer &src, uint32_t maxlen=0xFFFFFFFF);

	/// Copy from a string, assuming it is NUL-terminated, without
	/// overflowing, and ensuring that the result is properly NUL-termed.
	/// @param src source string
	/// @return length of resulting string
	uint32_t strncpy(const char *src);

	/// Copy from another buffer without overflowing, and append
	/// a NUL terminator, i.e. copy an unterminated string into
	/// a terminated string.
	/// @param src source buffer
	/// @param maxlen maximum number of bytes to copy from src
	/// @param length of resulting string
	uint32_t strncpy(const buffer &src, uint32_t maxlen=0xFFFFFFFF);

	/// determine the number of bytes that the buffers have in common
	/// at their start
	uint32_t common_prefix(const buffer &b) const;

private:

	uint8_t *ptr;
	uint32_t len;

	const buffer &operator=(const buffer &);

	static uint8_t canary;
};


/**
 * A buffer object that allocates and owns the memory that it refers to.
 */
template<uint32_t BUFSIZE>
class allocated_buffer : public buffer {
public:
	allocated_buffer()
		: buffer(&data[0], BUFSIZE)
	{}

	/// deep copy of a buffer, without overflowing
	template<uint32_t OTHERSIZE>
	explicit allocated_buffer(const allocated_buffer<OTHERSIZE> &o)
		: buffer(&data[0], BUFSIZE)
	{
		uint32_t sz=min(BUFSIZE, OTHERSIZE);
		memcpy(&data[0], (const uint8_t *) o, sz);
	}

private:

	uint8_t data[BUFSIZE];

	// no copying!
	const allocated_buffer &operator=(const allocated_buffer &);
};

#endif /* BUFFER_H_ */
