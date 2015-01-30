/*
 * utils.h
 *
 *  Created on: 1 sty 2015
 *      Author: mbialowa
 */

#ifndef UTILS_H_
#define UTILS_H_

#define PGMSTR(x) (__FlashStringHelper*)(x)

int getIntArg(const char* data, const char* key, int value = -1);
char getCharArg(const char* data, const char* key);

int freeRam ();

#endif /* UTILS_H_ */
