/*
 * keyb.h
 *
 *  Created on: 9 lut 2015
 *      Author: mbialowa
 */

#ifndef KEYB_H_
#define KEYB_H_

//***********************************************************************
// Plik: keyb.h
//
// Zaawansowana obsługa przycisków i klawiatur
// Wersja:    1.0
// Licencja:  GPL v2
// Autor:     Deucalion
// Email:     deucalion#wp.pl
// Szczegóły: http://mikrokontrolery.blogspot.com/2011/04/jezyk-c-biblioteka-obsluga-klawiatury.html
//
//***********************************************************************


#define KEY_PORT   PINB
#define KEY_DDR    DDRB

#define KEY0 ( 1 << PB0 )
#define KEY1 ( 1 << PB1 )
#define KEY2 ( 1 << PB2 )
#define KEY3 ( 1 << PB3 )
#define KEY4 ( 1 << PD6 )

#define KEY_ON_OFF  		  KEY4
#define KEY_SUB     		  KEY0
#define KEY_SUM_FRONT_RL      KEY1
#define KEY_CD   			  KEY2
#define KEY_AUX   			  KEY3
#define ANYKEY     (KEY0 | KEY1 | KEY2 | KEY3 | KEY4 )
#define KEY_MASK   (ANYKEY)

#define KBD_LOCK     1
#define KBD_NOLOCK   0

#define KBD_DEFAULT_ART   ((void *)0)

void ClrKeyb( int lock );

unsigned int GetKeys(void);

unsigned int KeysTime(void);

unsigned int IsKeyPressed(unsigned int mask);

unsigned int IsKey(unsigned int mask);

void KeybLock(void);

void KeybSetAutoRepeatTimes(unsigned short * AutoRepeatTab);

void KeybProc(void);

#endif /* KEYB_H_ */
