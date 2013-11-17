/*
 * EString.h
 *
 *  Created on: 17 лист. 2013
 *      Author: vladimir
 */

#ifndef ESTRING_H_
#define ESTRING_H_

#include <string>
#include "EMallocAllocator.h"

typedef std::basic_string<char, std::char_traits<char>, EMallocAllocator<char> > EString;

#endif /* ESTRING_H_ */
