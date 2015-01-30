#include "4ChannelAmp.h"
#include "menu.h"
#include "utils.h"
#include <avr/wdt.h>

//The setup function is called once at startup of the sketch
void setup(){
    /*
    byte vl=0;
    //eeprom_read_block(mymac, mymacEEPROM, sizeof(mymacEEPROM));
    Serial.begin(9600);
    for (byte i = 0; i < 7; ++i){
      vl = eeprom_read_byte(CONFIG_EEPROM_ADDR + i);
      Serial.print((unsigned char )vl, HEX);
      Serial.print(", ");
    }
    */
    Serial.begin(9600);

#ifdef DEBUG_AMP

    Serial.println("\n[memCheck]");
    Serial.println(freeRam());

#endif
    tda7448 = new TDA7448(TDA7448_I2C_ADDR);
    encoder = new ClickEncoder(A1, A2, A3, 2);

    lcd.begin(16,2);
    lcd.backlight();

    setupMenu();

    /*pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);*/

    pinMode(IRMP_BIT, INPUT);
    Timer1.initialize(IRMAP_US);
    Timer1.attachInterrupt(timer1Interrupt);

    lcd.print(F("Konfiguruje"));
    lcd.setCursor(0,1);
    lcd.print(F("siec. Czekaj ..."));

    if (ether.begin(sizeof Ethernet::buffer, networkMAC) == false){

#ifdef DEBUG_AMP

       Serial.println( F("Failed to access Ethernet controller"));

#endif
    }

    if (!ether.dhcpSetup()){

#ifdef DEBUG_AMP

       Serial.println(F("DHCP failed"));

#endif
       ampCurrentState.discoverDhcpIp = false;
    }

#ifdef DEBUG_AMP

     ether.printIp("My IP: ", ether.myip);
     ether.printIp("GW IP: ", ether.gwip);
     ether.printIp("DNS IP: ", ether.dnsip);

     Serial.println(F("\n[memCheck]"));
     Serial.println(freeRam());

#endif

     //default configuration
     ampCurrentState.inputSource = CD_DVD;
     ampCurrentState.muteFlag = false;
     ampCurrentState.networkSleepModeTime = millis();
     displayCurrentState();
     wdt_enable(WDTO_8S);


}

//Main loop
void loop(){

  handleSerialPort();
  handleNetworkSleepMode();
  handleNetwork();
  handleEncoder();
  handleEncoderClick();
  handleIRCommand();

  handleMenuTimeOut();
  handleAccelerateEncoderTimeOut();

  displayCurrentState();
  wdt_reset();
}


void static inline handleNetworkSleepMode(){

    if (ampCurrentState.networkSleepMode == true
            && ampCurrentState.networkSleepModeTime > 0
            && millis() - ampCurrentState.networkSleepModeTime >= NETWORK_DOWN_TIME){

        ether.powerUp();

#ifdef DEBUG_AMP

        Serial.println("true");
        Serial.println(millis());
        Serial.println(ampCurrentState.networkSleepModeTime);

#endif
        ampCurrentState.networkSleepMode = false;
        ampCurrentState.networkSleepModeTime = millis();
        return;
    }

    if (ampCurrentState.networkSleepMode == false
               && ampCurrentState.networkSleepModeTime > 0
               && millis() - ampCurrentState.networkSleepModeTime >= NETWORK_UP_TIME){

        ether.powerDown();

#ifdef DEBUG_AMP

        Serial.println("false");
        Serial.println(millis());
        Serial.println(ampCurrentState.networkSleepModeTime);

#endif
        ampCurrentState.networkSleepMode = true;
        ampCurrentState.networkSleepModeTime = millis();
        return;
     }

}


void setChannelVolume(uint8_t channel, int8_t level){

    ampCurrentState.refreshVolumeLevel = true;
    if(channel == MAIN_CHANNEL){
       ampCurrentState.channels.channel[MAIN_CHANNEL] = tda7448->normAttenuationLevelToMinMax(ampCurrentState.channels.channel[MAIN_CHANNEL] + level);
       for(uint8_t i = 0; i< ALL_CHANNELS - 1; i++){
           ampCurrentState.channels.channel[i] = tda7448->normAttenuationLevelToMinMax(ampCurrentState.channels.channel[i] + level);
       }
        tda7448->setChannelsAttenuationLeveldB(ampCurrentState.channels.channel);
    }else{
        ampCurrentState.channels.channel[channel] = tda7448->normAttenuationLevelToMinMax(level);
        tda7448->setChannelAttenuationdB(channel, ampCurrentState.channels.channel[channel]);
    }
}


void static inline handleEncoder() {

    value += encoder->getValue();

    //jesli sprzet jest wylaczony nie reaguj na zmiany encodera
    if (value != last && ampCurrentState.onOffFlag) {

        //obsluga poziomu glosnosci
        //dla wlaczonych ustawien nie zmieniaj glownego poziomu glosnosci
        if (!ampCurrentState.settingsMenuFlag && !ampCurrentState.muteFlag) {

            int8_t increment = (ampCurrentState.accelareateVol > ENCODER_ACCELERATE_COUNT) ? ATTENTUATION_INCREMENT_BY_5 : 1;

            if (last < value) {
                setChannelVolume(MAIN_CHANNEL, increment);
            } else {
                setChannelVolume(MAIN_CHANNEL, -increment);
            }

            ampCurrentState.accelareateVol ++;
            ampCurrentState.accelareateVolTime = millis();
        }

        //wybor opcji menu ustawien
        //przelaczamy sie pomiedzy menu
        if (ampCurrentState.settingsMenuFlag == true && ampCurrentState.channelVolumeFlag == false) {

            if (last < value) {
                ms.next(true);
            } else {
                ms.prev(true);
            }

            displaySettingsMenu();
        }

        //obsluga poziomu glosnosci dla konkretnego kanalu tda7448
        if (ampCurrentState.settingsMenuFlag == true && ampCurrentState.channelVolumeFlag == true) {

            uint8_t channelId = getCurrentMenuId();

            if (last < value) {
                ampCurrentState.channels.channel[channelId] = tda7448->normAttenuationLevelToMinMax(ampCurrentState.channels.channel[channelId] - 1);
            } else {
                ampCurrentState.channels.channel[channelId] = tda7448->normAttenuationLevelToMinMax(ampCurrentState.channels.channel[channelId] + 1);
            }

            displayVolumeLevel(ampCurrentState.channels.channel[channelId], REFRESH_LINE);
            tda7448->setChannelAttenuationdB(channelId, ampCurrentState.channels.channel[channelId]);

        }
        last = value;
    }
}


void static inline handleEncoderClick() {

    ClickEncoder::Button b = encoder->getButton();
    if (b != ClickEncoder::Open) {
       switch (b) {
          case ClickEncoder::Clicked:
            if (ampCurrentState.settingsMenuFlag == false) {
                ampCurrentState.settingsMenuFlag = true;
                ampCurrentState.settingsMenuOnTime = millis();
                ms.back();
                displaySettingsMenu();
            } else {
                if (ampCurrentState.channelVolumeFlag == false) {
                    ampCurrentState.channelVolumeFlag = true;
                    ms.select(true);
                }else {
                    ampCurrentState.channelVolumeFlag = false;
                    displaySettingsMenu();
                }
            }
            break;
       }
    }
}


void static inline handleNetwork(){

    if(ether.isLinkUp() && !ampCurrentState.networkSleepMode){

        if(!ampCurrentState.discoverDhcpIp){

            if(ether.dhcpSetup()){
                ampCurrentState.discoverDhcpIp = true;

#ifdef DEBUG_AMP

                ether.printIp("My IP: ", ether.myip);
                ether.printIp("GW IP: ", ether.gwip);
                ether.printIp("DNS IP: ", ether.dnsip);

#endif

            }
        }

      word len = ether.packetReceive();
      word pos = ether.packetLoop(len);

      if (pos){  // check if valid tcp data is received
          bfill = ether.tcpOffset();
          char* data = (char *) Ethernet::buffer + pos;
          char cmd = getCharArg(data, "cmd");
          int8_t vol = getIntArg(data, "vol");
          uint8_t channel = getIntArg(data, "channel");

          processCommand(cmd);
          if(channel >= 0){
              setChannelVolume(channel, vol);
          }

#ifdef DEBUG_AMP

      Serial.println(data);
      Serial.println(cmd);
      Serial.println(vol);
      Serial.println(channel);

#endif

      bfill.emit_p(okHeader);
      ether.httpServerReply(bfill.position()); // send web page data
      }
    }
}


void static inline displaySettingsMenu() {

    lcd.clear();
    // Display the menu
    Menu const* cp_menu = ms.get_current_menu();
    lcd.print(cp_menu->get_selected()->get_name());

    uint8_t menuId = cp_menu->get_cur_menu_component_num();

    //menu 1-4 has vol level, menu 5-6 does not
    //check if is necessary to display vol level
    if (menuId < cp_menu->get_num_menu_components() - 2){
        displayVolumeLevel(ampCurrentState.channels.channel[menuId], REFRESH_LINE)	;
    }
}


void static inline displayCurrentState(){

    if(ampCurrentState.refreshLCD){
//LCD 1 line
        lcd.clear();
        switch(ampCurrentState.inputSource){
            case CD_DVD:
                lcd.print(PGMSTR(input_cd_dvd_text));
                break;
            case AUX:
                lcd.print(PGMSTR(input_aux_text));
                break;
        }

        if(ampCurrentState.inputLRSumFlag){
            lcd.setCursor(7,0);
            lcd.print(PGMSTR(sum_lr_sub_text));
        }

        if(ampCurrentState.subOnlyFlag){
            lcd.setCursor(13,0);
            lcd.print(PGMSTR(sub_text));
        }
//LCD 2 line
        displayMainVolumeLevelOrMute(REFRESH_LINE);

        ampCurrentState.refreshLCD         = false;
        ampCurrentState.channelVolumeFlag  = false;
        ampCurrentState.settingsMenuFlag   = false;
        ampCurrentState.refreshVolumeLevel = false;

    }

   if (ampCurrentState.refreshVolumeLevel){
       ampCurrentState.refreshVolumeLevel = false;
       displayMainVolumeLevelOrMute(NO_LINE_REFRESH);
   }
}


void static displayVolumeLevel(uint8_t volLevel, uint8_t refreshWholeLine) {

    if(refreshWholeLine){
       lcd.setCursor(0,1);
       lcd.print(PGMSTR(poziom_text));
    }else{
       lcd.setCursor(7,1);
       lcd.print(F("    "));
       lcd.setCursor(7,1);
    }

    if (volLevel > 0) {
       lcd.print(F("-"));
    }

    lcd.print(volLevel);
    lcd.print(F(" dB   "));
}


void static displayMainVolumeLevelOrMute(uint8_t refreshWholeLineFlag){

   if(ampCurrentState.muteFlag){
      lcd.clearLine(1);
      lcd.setCursor(0,1);
      lcd.print(PGMSTR(mute_text));
   }else{
      displayVolumeLevel(ampCurrentState.channels.channel[MAIN_CHANNEL], refreshWholeLineFlag);
   }
}


void static muteCmd(){

    ampCurrentState.muteFlag = !ampCurrentState.muteFlag;
    ampCurrentState.refreshLCD = true;

    if(ampCurrentState.muteFlag){
        tda7448->mute();
    }else{
        tda7448->setChannelsAttenuationLeveldB(ampCurrentState.channels.channel);
    }

}


void static powerOnOffCmd(){

    ampCurrentState.onOffFlag = !ampCurrentState.onOffFlag;
    if(ampCurrentState.onOffFlag){
        ether.powerUp();
        lcd.on();
    }else{
       // set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        lcd.off();
        //attachInterrupt(0, wakeUpNow, LOW);
        ether.powerDown();
        //sleep_enable();
        //sleep_mode();

        //sleep_disable();
        //detachInterrupt(0);
    }
}


void static subOnlyCmd(){

    ampCurrentState.subOnlyFlag = !ampCurrentState.subOnlyFlag;
    if(ampCurrentState.subOnlyFlag ){

    }else{

    }
}


void static sourceSelectCmd(InputSource  source){

    ampCurrentState.inputSource = source;
    ampCurrentState.refreshLCD = true;

    switch(source){
        case AUX:

            break;

        case CD_DVD:

            break;
    }
}


void static sumFrontLRCmd(){

    ampCurrentState.inputLRSumFlag = !ampCurrentState.inputLRSumFlag;

    if(ampCurrentState.inputLRSumFlag){

    }else{

    }

}


void static processCommand(char inChar){

   if(ampCurrentState.onOffFlag){
     switch (inChar) {

        case 'w': //onOff
                powerOnOffCmd();
                break;

        case 's':
                subOnlyCmd();
                break;

        case 'a':
                sourceSelectCmd(AUX);
                break;

        case 'd':
                sourceSelectCmd(CD_DVD);
                break;

        case 'm':
                muteCmd();
                break;

        case 'i':
                sumFrontLRCmd();
                break;

        default:
          break;
     }
     ampCurrentState.refreshLCD = 1;
   }

}


void static inline handleSerialPort() {

  char inChar;
  if((inChar = Serial.read())>0){
      processCommand(inChar);
  }
}

/* helper function: attachInterrupt wants void(), but irmp_ISR is uint8_t() */
void static timer1Interrupt(){

  irmp_ISR();
  encoder->service();
}


void onResetMenu(MenuItem* p_menu_item){

    for(uint8_t i = 0; i< ALL_CHANNELS; i++){
       ampCurrentState.channels.channel[i] = DEFAULT_CHANNAL_ATTENTUATION;
    }
    onBackToMainMenu(p_menu_item);
}


void onBackToMainMenu(MenuItem* p_menu_item){

    ampCurrentState.channelVolumeFlag     = false;
    ampCurrentState.settingsMenuFlag    = false;
    ampCurrentState.refreshLCD 		    = true;
    ampCurrentState.settingsMenuOnTime  = false;
}


void onMenuItem(MenuItem* p_menu_item){

    uint8_t channelId = getCurrentMenuId();

    lcd.setCursor(0,0);
    lcd.print(F("->"));
    lcd.print(p_menu_item->get_name());

    displayVolumeLevel(ampCurrentState.channels.channel[channelId], NO_LINE_REFRESH);
}


void static inline handleMenuTimeOut(){

    if(ampCurrentState.settingsMenuOnTime > 0 && millis() - ampCurrentState.settingsMenuOnTime >= MENU_TIMEOUT){
        onBackToMainMenu(NULL);
    }
}


void static inline handleAccelerateEncoderTimeOut(){

    if(ampCurrentState.accelareateVol > ENCODER_ACCELERATE_COUNT && millis() - ampCurrentState.accelareateVolTime >= ENCODER_ACCELERATE_TIME){
        ampCurrentState.accelareateVol = 0;
    }
}


void static inline handleIRCommand(){

    if (irmp_get_data (&irmp_data)){
         if (irmp_data.protocol == IRMP_DENON_PROTOCOL && ampCurrentState.settingsMenuFlag == false && (ampCurrentState.onOffFlag || irmp_data.command == 0x338)){

             uint8_t incrementBy = (irmp_data.flags == 1) ? ATTENTUATION_INCREMENT_BY_5 : 1;

             switch(irmp_data.command){
                 case 0x104:
                           sourceSelectCmd(CD_DVD);
                           break;
                 case 0x84:
                           sourceSelectCmd(AUX);
                           break;
                 case 0x338:
                           powerOnOffCmd();
                           break;
                 case 0x1AC: //down
                           setChannelVolume(MAIN_CHANNEL, -incrementBy);
                           break;
                 case 0x2AC: //up
                           setChannelVolume(MAIN_CHANNEL, incrementBy);
                           break;
                 case 0x344: //mute
                           muteCmd();
                           break;

             }
         }

#ifdef DEBUG_AMP

      Serial.print(" C:");
      Serial.print(irmp_data.command, HEX);
      Serial.print(" ");
      Serial.print(irmp_data.flags, HEX);
      Serial.print(" ");
      Serial.print(irmp_data.protocol, HEX);
      Serial.print("");

#endif

    }
}

