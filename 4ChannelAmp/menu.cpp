/*
 * menu.cpp
 *
 *  Created on: 27 gru 2014
 *      Author: mbialowa
 */
#include "menu.h"

MenuSystem ms;
Menu mm(("MG"));
MenuItem mm_mi1("Subwoofer  ");
MenuItem mm_mi2("Tyl Prawy  ");
MenuItem mm_mi3("Tyl Lewy   ");
MenuItem mm_mi4("Centralny  ");
MenuItem mm_mi5("Reset poziomow");
MenuItem mm_mi6("Powrot  ");

void setupMenu(){
   mm.add_item(&mm_mi1, &onMenuItem);
   mm.add_item(&mm_mi2, &onMenuItem);
   mm.add_item(&mm_mi3, &onMenuItem);
   mm.add_item(&mm_mi4, &onMenuItem);
   mm.add_item(&mm_mi5, &onResetMenu);
   mm.add_item(&mm_mi6, &onBackToMainMenu);
   ms.set_root_menu(&mm);
}


uint8_t getCurrentMenuId(){
    Menu const* cp_menu = ms.get_current_menu();
    return cp_menu->get_cur_menu_component_num();
}
