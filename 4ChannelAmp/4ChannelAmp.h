
// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _4ChannelAmp_H_
#define _4ChannelAmp_H_


#include <Platform.h>
#include <TimerOne.h>
#include <EtherCard.h>
#include <avr/sleep.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ClickEncoder.h>

#include <avr/eeprom.h>
#include "TDA7448.h"
#include "messages.h"


void loop();
void setup();

#ifdef __cplusplus
extern "C" {

#include <irmp.h>
#define IRMAP_US (1000000 / F_INTERRUPTS)
IRMP_DATA irmp_data;

}
#endif

#define CONFIG_EEPROM_ADDR ((byte*) 0x00)

// ethernet interface mac address
static byte networkMAC[] = {0x74,0x69,0x69,0x2D,0x30,0x33} ;
//byte mymacEEPROM[6] EEMEM;
//static byte mymac[] =       { 0x74,0x69,0x69,0x2D,0x30,0x31 };
//static byte mymac1[1024] EEMEM ;

byte Ethernet::buffer[300];
BufferFiller bfill;

typedef enum InputSource {
    CD_DVD,
    AUX
} tInputSource;

#define DEFAULT_CHANNAL_ATTENTUATION 0x3C
#define NETWORK_DOWN_TIME            15000
#define NETWORK_UP_TIME              60000
#define ENCODER_ACCELERATE_TIME      1500
#define ENCODER_ACCELERATE_COUNT 20
#define ATTENTUATION_INCREMENT_BY_5  5


#define REFRESH_LINE TRUE
#define NO_LINE_REFRESH FALSE

typedef struct TDA7448Channels {
    uint8_t channel[ALL_CHANNELS];

    TDA7448Channels(){
        memset(channel, DEFAULT_CHANNAL_ATTENTUATION, ALL_CHANNELS);
    }
} tTDA7448Channels;

typedef struct AMPCurrentState {

    bool     settingsMenuFlag      	;
    bool     channelVolumeFlag 		;
    bool     onOffFlag         		;
    bool     subOnlyFlag       		;
    bool     inputLRSumFlag    		;
    bool     muteFlag          		;
    tInputSource  inputSource 	    ;
    bool     networkDataFlag		;
    bool     remoteControlFlag		;
    uint32_t settingsMenuOnTime		;
    bool     refreshLCD				;//refresh whole LCD
    bool     refreshVolumeLevel     ;//refresh only volume section, NO whole line refresh
    bool     discoverDhcpIp			;
    bool     networkSleepMode       ;
    uint32_t networkSleepModeTime   ;
    uint8_t  accelareateVol         ;
    uint32_t accelareateVolTime     ;
    tTDA7448Channels channels       ;

    AMPCurrentState():
        settingsMenuFlag(false),
        channelVolumeFlag(false),
        onOffFlag(true),
        subOnlyFlag(true),
        inputLRSumFlag(true),
        muteFlag(false),
        inputSource(CD_DVD),
        networkDataFlag(false),
        remoteControlFlag(false),
        settingsMenuOnTime(false),
        refreshVolumeLevel(false),
        refreshLCD(true),
        networkSleepMode(false),
        discoverDhcpIp(true),
        networkSleepModeTime(0),
        accelareateVol(false),
        accelareateVolTime(0)
    {

    }

} tAMPCurrentState;

tAMPCurrentState ampCurrentState ;//__attribute__((section(".eeprom")));

ClickEncoder *encoder;
TDA7448 *tda7448 ;
int8_t last =-1, value;

// set the LCD address to 0x27 for a 16 chars 2 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address



void static processCommand(char inChar);
void static sourceSelectCmd(InputSource  source);
void static powerOnOffCmd();
void static subOnlyCmd();
void static sumFrontLRCmd();
void static muteCmd();

void static displayMainVolumeLevelOrMute(uint8_t refreshWholeLineFlag);
void static displayVolumeLevel(uint8_t volLevel, uint8_t refreshWholeLine) ;

void static inline displayCurrentState();
void static inline displaySettingsMenu();
void static inline handleMenuTimeOut();
void static inline handleIRCommand();
void static inline handleSerialPort();
void static inline handleEncoder();
void static inline handleEncoderClick();
void static inline handleNetwork();
void static inline handleNetworkSleepMode();
void static inline handleAccelerateEncoderTimeOut();

void static timer1Interrupt();
void static wakeUpNow(){};



#endif /* _4ChannelAmp_H_ */
