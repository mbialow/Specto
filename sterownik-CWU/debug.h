/*
 * debug.h
 *
 *  Created on: 18 mar 2015
 *  Author: Marcin Bialowas -  mbialow@gmail.com
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <avr/io.h>

void printTmep(unsigned long tempOdczytanaCzescCalkowita);


void send_chars(uint8_t *p);

/*
 * send_uart
 * Sends a single char to UART without ISR
 */
void send_uart(uint8_t c);


uint8_t * ltoaz(unsigned long val, uint8_t *buf, int radix, int dig) ;


void init_uart();


#endif /* DEBUG_H_ */
