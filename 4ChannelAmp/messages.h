/*
 * messages.h
 *
 *  Created on: 27 gru 2014
 *      Author: mbialowa
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include <Platform.h>

const u8 input_cd_dvd_text[] PROGMEM = "CD/DVD";
const u8 input_aux_text[] PROGMEM = "AUX";
const u8 mute_text[] PROGMEM = "MUTE";
const u8 sub_text[] PROGMEM = "SUB";
const u8 sum_lr_sub_text[] PROGMEM = "PL+PP";
const u8 poziom_text[] PROGMEM = "Poziom ";

const char okHeader[] PROGMEM =
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

#endif /* MESSAGES_H_ */
