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
 * @file buffer.cpp
 * Bounds-checked memory accessors.
 */

#include "buffer.h"

uint8_t buffer::canary=0;

void buffer::assign(const buffer &o, uint32_t off, uint32_t sublen)
{
	// totally out of bounds, make invalid
	if(off >= o.len){
		ptr=(uint8_t *) 0;
		len=0;
	}
	else{
		// at least partially in-bounds
		ptr=o.ptr+off;
		len=min(o.len-off, sublen);
	}
}

uint32_t buffer::copy(const buffer &src, uint32_t maxlen)
{
	uint32_t sz=src.len;
	if(maxlen < sz)
		sz=maxlen;
	if(len < sz)
		sz=len;

	memcpy(ptr, src.ptr, sz);

	return sz;
}

uint32_t buffer::strncpy(const char *src)
{
	uint32_t sz=strlen(src);
	if(len-1 < sz)
		sz=len-1;		// leave room for NUL

	memcpy(ptr, src, sz);
	ptr[sz]='\0';

	return sz;
}

uint32_t buffer::strncpy(const buffer &src, uint32_t maxlen)
{
	uint32_t sz=src.len;
	if(maxlen < sz)
		sz=maxlen;
	if(len-1 < sz)
		sz=len-1;		// leave room for NUL

	memcpy(ptr, src.ptr, sz);
	ptr[sz]='\0';

	return sz;
}


uint32_t buffer::common_prefix(const buffer &b) const
{
	uint32_t maxlen=min(length(), b.length());

	for(uint32_t i=0;i<maxlen;++i){
		if(ptr[i] != b[i]){
			return i;
		}
	}
	return maxlen;
}
