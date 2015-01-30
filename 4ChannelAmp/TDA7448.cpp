/*
 * TDA7448.cpp
 *
 *  Created on: 24 sie 2014
 *      Author: mbialowa
 */

#include "TDA7448.h"
#include "Wire.h"


TDA7448::TDA7448(uint8_t i2cAddr){
    this->_i2cAddr = i2cAddr;
    //this->_initialised = true;
}


void TDA7448::mute(){

    Wire.beginTransmission(_i2cAddr);
    //function
    Wire.write(INCREMENTAL_SPEAKER_ATTENUATION_OUT_1);
    /*
    //channel1
    Wire.write(CHANNEL_MUTE);
    //channel2
    Wire.write(CHANNEL_MUTE);
    //channel3
    Wire.write(CHANNEL_MUTE);
    //channel4
    Wire.write(CHANNEL_MUTE);
    //channel5
    Wire.write(CHANNEL_MUTE);
    //channel6
    Wire.write(CHANNEL_MUTE);*/

    for(uint8_t i = 0; i< ALL_CHANNELS - 1; i++){
        Wire.write(CHANNEL_MUTE);
    }

    /* status =*/ Wire.endTransmission ();
}


void TDA7448::setChannelAttenuationdB(const uint8_t channelId, uint8_t level){
    Wire.beginTransmission(_i2cAddr);
    Wire.write(channelId);
    Wire.write(convertTodB(level));
    Wire.endTransmission();
}


void TDA7448::setChannelsAttenuationLeveldB(const uint8_t *channel){
    uint8_t tempChannel[ALL_CHANNELS] = { };

    Wire.beginTransmission(_i2cAddr);
    Wire.write(INCREMENTAL_SPEAKER_ATTENUATION_OUT_1);

    for(uint8_t i = 0; i< ALL_CHANNELS - 1 ;i++){
        tempChannel[i] = convertTodB(channel[i]);
    }

    Wire.write(tempChannel, ALL_CHANNELS - 1);
    Wire.endTransmission();
}

/*
  according to the TDA7448 datasheet.
  this function translate the 0--79dB to a data that TDA7448 can be accepted
  the real attenuation value = lower 3bit + (8 * higher 5 bits)
  for example
   input: 9
   out   : 0x05
*/
uint8_t TDA7448::convertTodB(uint8_t volLevel){

    uint8_t lower3  = volLevel % 8;
    uint8_t higher5 = volLevel / 8;
    uint8_t ret = ( (higher5<<3) | lower3);

    return ret;
}


uint8_t TDA7448::normAttenuationLevelToMinMax(int8_t level){

    if(level <= 0 ) return MAX_VOL_LEVEL;
    if(level > MIN_VOL_LEVEL) return MIN_VOL_LEVEL;

    return level;
}

