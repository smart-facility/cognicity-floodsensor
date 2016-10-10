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
 * @file dtostrf.h
 * Number-to-string conversions.
 */

#ifndef _DTOSTRF_H
#define _DTOSTRF_H

#include "cmsis_device.h"

/// convert an int to a string
/// @param i the integer to convert
/// @param base the radix, 2..36
/// @param width the total string length (not incl NUL) to fill; if this is insufficient then the beginning of the string will be truncated
/// @param fill the pre-fill character
/// @param buf where to store the string; must contain width+1 bytes of space
/// @return the string so created
/// @bug will not work properly for most-negative-number
char *itostr(int32_t i, unsigned base, unsigned width, char fill, char *buf);

/// convert an unsigned to a string
/// @param i the integer to convert
/// @param base the radix, 2..36
/// @param width the total string length (not incl NUL) to fill; if this is insufficient then the beginning of the string will be truncated
/// @param fill the pre-fill character
/// @param buf where to store the string; must contain width+1 bytes of space
/// @return the string so created
char *utostr(uint32_t i, unsigned base, unsigned width, char fill, char *buf);

/// convert fixed-point to a string
/// @param i the integer to convert
/// @param the fixed-point factor (e.g. 16 for an x.4 format)
/// @param base the radix, 2..36
/// @param width the total string length (not incl NUL) to fill; if this is insufficient then the beginning of the string will be truncated
/// @param prec the post-decimal precision; must be enough to represent factor in specified base
/// @param fill the pre-fill character
/// @param buf where to store the string; must contain width+1 bytes of space
/// @return the string so created
char *dtostrx(int32_t i, unsigned point, unsigned base, unsigned width, unsigned prec, char fill, char *buf);

/// convert unsigned fixed-point to a string
/// @param i the integer to convert
/// @param the fixed-point factor (e.g. 16 for a 28.4 format)
/// @param base the radix, 2..36
/// @param width the total string length (not incl NUL) to fill; if this is insufficient then the beginning of the string will be truncated
/// @param prec the post-decimal precision; must be enough to represent factor in specified base
/// @param fill the pre-fill character
/// @param buf where to store the string; must contain width+1 bytes of space
/// @return the string so created
char *dtostrux(uint32_t i, unsigned point, unsigned base, unsigned width, unsigned prec, char fill, char *buf);

/// format a float as a string in fixed-width
/// @param x the value to convert
/// @param width the minimum string width not include leading - and trailing NUL
/// @param prec the decimal precision
/// @param buf the output buffer; must contain at least width+2 chars; more if x is large (11+prec)
char *dtostrf(float x, unsigned width, unsigned prec, char fill, char *buf);

/// iteratively (O(i)) compute an integer power of 10.0 as a float
/// this is really slow but small code, use it only for small values of i,
/// e.g. when performing output formatting
/// @param i the exponent, base 10
/// @return 10.0 ^^ i
float pow10fi(int i);

#endif
