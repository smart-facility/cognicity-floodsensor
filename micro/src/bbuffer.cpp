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
 * @file bbuffer.cpp
 * Bounded Buffer implementation
 */

#include "bbuffer.h"

bounded_buffer::bounded_buffer(buffer &b)
 : buf(b), readptr(0), writeptr(0), used(0), avail(buf.length())
{
}

#ifndef SINGLETHREAD
void bounded_buffer::insert(const buffer &b)
{
	uint32_t inserted=0;

	// byte at a time, blocking as we go
	while(inserted < b.length()){
		avail.dec();
		buf[writeptr++]=b[inserted++];
		if(writeptr >= buf.length()){
			writeptr=0;
		}
		used.inc();
	}
}
#endif

uint32_t bounded_buffer::insertFromInterrupt(const buffer &b)
{
	uint32_t can=min(b.length(), space());
#if SINGLETHREAD
	avail-=can;
#else
	avail.decFromInterrupt(can);
#endif
	for(uint32_t c=0;c<can;++c){
		buf[writeptr++]=b[c++];
		if(writeptr >= buf.length()){
			writeptr=0;
		}
	}
	used.incFromInterrupt(can);

	return can;
}

bool bounded_buffer::insertFromInterrupt(uint8_t b)
{
#ifndef SINGLETHREAD
	if(avail.decFromInterrupt()){
#else
	if(avail >= 1){
		--avail;
#endif
		buf[writeptr++]=b;
		if(writeptr >= buf.length()){
			writeptr=0;
		}
		used.incFromInterrupt();
		return true;
	}
	return false;
}

void bounded_buffer::retrieve(buffer &b)
{
	uint32_t retrieved=0;

	while(retrieved < b.length()){
		used.dec();
		b[retrieved++]=buf[readptr++];
		if(readptr >= buf.length()){
			readptr=0;
		}
#if SINGLETHREAD
		++avail;
#else
		avail.inc();
#endif
	}
}

buffer bounded_buffer::retrieveNonBlocking(buffer &b)
{
	buffer result(b, 0, min(b.length(), data()));

	used.dec(result.length());
	for(uint32_t i=0;i<result.length();++i){
		result[i]=buf[readptr++];
		if(readptr >= buf.length()){
			readptr=0;
		}
	}
#if SINGLETHREAD
	avail += result.length();
#else
	avail.inc(result.length());
#endif
	return result;
}
