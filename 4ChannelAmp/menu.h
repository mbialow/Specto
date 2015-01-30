/*
 * menu.h
 *
 *  Created on: 27 gru 2014
 *      Author: mbialowa
 */

#ifndef MENU_H_
#define MENU_H_

#include <MenuSystem.h>

// Menu variables
#define MENU_TIMEOUT 20000

extern MenuSystem ms;
extern Menu mm;
extern MenuItem mm_mi1;
extern MenuItem mm_mi2;
extern MenuItem mm_mi3;
extern MenuItem mm_mi4;
extern MenuItem mm_mi5;
extern MenuItem mm_mi6;


void setupMenu();

// Menu callback function
void onBackToMainMenu(MenuItem* p_menu_item);
void onMenuItem(MenuItem* p_menu_item);
void onResetMenu(MenuItem* p_menu_item);
uint8_t getCurrentMenuId();

#endif /* MENU_H_ */
