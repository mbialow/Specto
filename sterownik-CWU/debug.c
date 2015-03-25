/*
 * debug.h
 *
 *  Created on: 18 mar 2015
 *  Author: Marcin Bialowas -  mbialow@gmail.com
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "debug.h"
#include <avr/io.h>
#define BAUD 4800
#include <util/setbaud.h>


uint8_t * ret = 0;
uint8_t buffer[10];

/*
 * send_uart
 * Sends a single char to UART without ISR
 */
void send_uart(uint8_t c) {
    // wait for empty data register
    while (!(UCSRA & (1 << UDRE)))
    ;
    // set data into data register
    UDR = c;
}

void send_chars(uint8_t *p) {
    while (*p) {
        send_uart(*p++);
    }
}

uint8_t * ltoaz(unsigned long val, uint8_t *buf, int radix, int dig) {
    char tmp[32];
    int i = 0;

    if ((radix >= 2) && (radix <= 16)) {
        do {
            tmp[i] = (val % radix) + '0';
            if (tmp[i] > '9')
            tmp[i] += 7;
            val /= radix;
            i++;
        }while (val);

        while ((i < dig) && (i < 32)) {
            tmp[i++] = '0';
        }

        while (i) {
            *buf++ = tmp[i - 1];
            i--;
        }
    }
    *buf = 0;
    return buf;
}

void printTmep(unsigned long tempOdczytanaCzescCalkowita) {

    ret = buffer;
    ret = ltoaz(tempOdczytanaCzescCalkowita, ret, 10, 0);
    //*ret++ = '.';
    //   ret = ltoaz( Ulamek, ret, 10, 2);
    *ret++ = 'C';
    *ret++ = '\n';
    *ret = '\0';

    send_chars(buffer);
}





void init_uart() {

    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;

    // enable receive and transmit
    UCSRB = (1 << RXEN) | (1 << TXEN);// | (1 << RXCIE);
    // set frame format
    UCSRC = (1 << USBS) | (1 << UCSZ0) | (1 << UCSZ1);// asynchron 8n1
}

#endif /* DEBUG_H_ */
