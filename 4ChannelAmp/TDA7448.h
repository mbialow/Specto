/*
 * TDA7448.h
 *
 *  Created on: 24 sie 2014
 *      Author: mbialowa
 */

#ifndef TDA7448_H_
#define TDA7448_H_
#include <inttypes.h>

/*
 * FUNCTION SELECTION: subaddress
 * MSB LSB
SUBADDRESS
D7 D6 D5 D4 D3 D2 D1 D0
X  X  X  B  0  0  0  0 SPEAKER ATTENUATION OUT 1
X  X  X  B  0  0  0  1 SPEAKER ATTENUATION OUT 2
X  X  X  B  0  0  1  0 SPEAKER ATTENUATION OUT 3
X  X  X  B  0  0  1  1 SPEAKER ATTENUATION OUT 4
X  X  X  B  0  1  0  0 SPEAKER ATTENUATION OUT 5
X  X  X  B  0  1  0  1 SPEAKER ATTENUATION OUT 6
X  X  X  B  0  1  1  0 NOT USED
X  X  X  B  0  1  1  1 NOT USED

B=1: INCREMENTAL BUS; ACTIVE
B=0: NO INCREMENTAL BUS
X= DON’T CARE
 *
 *
 *
 *
 *
 *MSB LSB
SPEAKER ATTENUATION
D7 D6 D5 D4 D3 D2 D1 D0
               0  0  0 0dB
               0  0  1 -1dB
               0  1  0 -2dB
               0  1  1 -3dB
               1  0  0 -4dB
               1  0  1 -5dB
               1  1  0 -6dB
               1  1  1 -7dB
0 0 0 0 0 -0dB
0 0 0 0 1 -8dB
0 0 0 1 0 -16dB
0 0 0 1 1 -24dB
0 0 1 0 0 -32dB
0 0 1 0 1 -40dB
0 0 1 1 0 -48dB
0 0 1 1 1 -56dB
0 1 -64dB
1 0 -72dB
1 1 MUTE
 *
 *
 *
 *
 *
 *
 */

#define INCREMENTAL_SPEAKER_ATTENUATION_OUT_1 0x10
#define SPEAKER_ATTENUATION_OUT_1 0x00
#define CHANNEL_MUTE 0xFF
#define TDA7448_I2C_ADDR 0x88

#define MIN_VOL_LEVEL 79
#define MAX_VOL_LEVEL 0
//max 6 channels are implemented in tda7448, 7 is man channel
//i'm using only 4 of them
#define ALL_CHANNELS 5
//main channel -  index from 0 to 5 and 6 as main channel
#define MAIN_CHANNEL (ALL_CHANNELS - 1)


class TDA7448 {

private:
    uint8_t _i2cAddr;     // I2C address
   // bool _initialised; // Initialized object
    uint8_t convertTodB(uint8_t volLevel);

public:
    //0x88 default
    //0x8A addr connected to supplay
    TDA7448(uint8_t i2cAddr);
    void mute();
    void setChannelsAttenuationLeveldB(const uint8_t *channel);
    void setChannelAttenuationdB(const uint8_t channelId, uint8_t level);
    uint8_t normAttenuationLevelToMinMax(int8_t level);

};

#endif /* TDA7448_H_ */
