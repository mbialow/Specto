/*
 * utils.c
 *
 *  Created on: 1 sty 2015
 *      Author: mbialowa
 */
#include <EtherCard.h>

int getIntArg(const char* data, const char* key, int value ) {

  char temp[10];
  if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
    value = atoi(temp);
  return value;
}

char getCharArg(const char* data, const char* key) {

  char temp[10];
  if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
    return temp[0];

  return 0;
}

#ifdef DEBUG_AMP

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#endif
