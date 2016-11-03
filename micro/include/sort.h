/* -*- C++ -*- */
/*
    Copyright (C) 2016 William Brodie-Tyrrell
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
 * @file sort.h
 * Sorting functions.
 */
#ifndef SORT_H_
#define SORT_H_

/**
 * Selection sort, in-place.
 * O(n^2) but OK for small data, and is small code.
 *
 * @param T the object-type being sorted.  Must be assignable & have operator<
 * @param p pointer to the objects to sort
 * @param count number of objects
 */
template<typename T>
void selection_sort(T *p, size_t count)
{
	for(size_t i=0;i<count-1;++i){

		// find "smallest"
		T minval=p[i];
		size_t minpos=i;
		for(size_t j=i+1;j<count;++j){
			if(p[j] < minval){
				minpos=j;
				minval=p[j];
			}
		}

		// swap; we already have p[j] stored in minval.  OK if i==j.
		p[minpos]=p[i];
		p[i]=minval;
	}
}


#endif /* SORT_H_ */
