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
 * @file dtostrf.cpp
 * Number-to-string conversions.
 */

#include "dtostrf.h"
#include <math.h>
#include <assert.h>

static constexpr const char *HEXCHARS="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char *itostr(int32_t i, unsigned base, unsigned width, char fill, char *buf)
{
	assert(base > 1 && base <= 36);
	assert(width >= 2);
	assert(buf != 0);

	int stopat=0;
	buf[width]='\0';
	if(i < 0){
		i=-i;
		buf[0]='-';
		stopat=1;
	}
	int p0=width-1;
	int p=p0;
	for(;p >= stopat && (i != 0 || p >= p0); --p, i /= base){
		buf[p]=HEXCHARS[i % base];
	}
	for(;p >= stopat; --p){
		buf[p]=fill;
	}

	return buf;
}

char *utostr(uint32_t i, unsigned base, unsigned width, char fill, char *buf)
{
	assert(base > 1 && base <= 36);
	assert(width >= 1);
	assert(buf != 0);

	buf[width]='\0';
	int p0=width-1;
	int p=p0;
	for(;p >= 0 && (i != 0 || p >= p0); --p, i /= base){
		buf[p]=HEXCHARS[i % base];
	}
	for(;p >= 0; --p){
		buf[p]=fill;
	}

	return buf;
}

char *dtostrx(int32_t x, unsigned point, unsigned base, unsigned width, unsigned prec, char fill, char *buf)
{
	assert((prec > 0 && width >= prec+3) || (prec == 0 && width >= 2));
	assert(buf != 0);

	bool neg=x<0;
	int32_t y=neg ? -x : x;
	uint32_t ipart=y / point;
	uint32_t power=1;
	for(unsigned k=0;k<prec;++k)
		power*=base;
	uint32_t dpart=uint32_t(uint64_t(y - ipart*point)*power / point);

	// convert int-part
	unsigned ilen=(prec > 0) ? width-prec-1 : width;
	itostr(neg ? -ipart : ipart, base, ilen, fill, buf);
	// convert decimal-part
	if(prec > 0){
		buf[ilen]='.';
		utostr(dpart, base, prec, '0', buf+ilen+1);
	}

	return buf;
}

char *dtostrux(uint32_t x, unsigned point, unsigned base, unsigned width, unsigned prec, char fill, char *buf)
{
	assert((prec > 0 && width >= prec+2) || (prec == 0 && width >= 1));
	assert(buf != 0);

	uint32_t ipart=x / point;
	uint32_t power=1;
	for(unsigned k=0;k<prec;++k)
		power*=base;
	uint32_t dpart=uint32_t(uint64_t(x - ipart*point)*power / point);

	// convert int-part
	unsigned ilen=(prec > 0) ? width-prec-1 : width;
	utostr(ipart, base, ilen, fill, buf);
	// convert decimal-part
	if(prec > 0){
		buf[ilen]='.';
		utostr(dpart, base, prec, '0', buf+ilen+1);
	}

	return buf;
}

float pow10fi(int i)
{
	float result=1.0f;
	if(i > 0){
		for(int k=0;k<i;++k){
			result *= 10.0f;
		}
	}
	else{
		for(int k=0;k<-i;++k){
			result *= 0.1f;
		}
	}
	return result;
}

char *dtostrf(float x, unsigned width, unsigned prec, char fill, char *buf)
{
	assert((prec > 0 && width >= prec+3) || (prec == 0 && width >= 2));
	assert(buf != 0);

	bool neg=x<0.0f;
	float y=neg ? -x : x;
	uint32_t ipart=uint32_t(truncf(y));
	uint32_t dpart=uint32_t(truncf(pow10fi(float(prec))*(y-float(ipart))));

	// convert int-part
	unsigned ilen=(prec > 0) ? width-prec-1 : width;
	itostr(neg ? -ipart : ipart, 10, ilen, fill, buf);
	if(prec > 0){
		buf[ilen]='.';
		utostr(dpart, 10, prec, '0', buf+ilen+1);
	}

	return buf;
}


