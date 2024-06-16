#include "TEA6330T.h"

void TEA6330T::syncWithTEA6330T(){
    writeToTEA6330T(VOL_LEFT, volume_l);
    writeToTEA6330T(VOL_RIGHT, volume_r);
    writeToTEA6330T(BASS, bass_gain);
    writeToTEA6330T(TREBLE, treble_gain);
    writeToTEA6330T(FADER, fader_rear);
    writeToTEA6330T(FADER, fader_front);
    
    uint8_t gu = global_mute ? GLOBAL_MUTE_ON : GLOBAL_MUTE_OFF;
    uint8_t eq = equalizer_takeover ? EQUALIZER_TAKEOVER_ON : EQUALIZER_TAKEOVER_OFF;
    writeToTEA6330T(AUDIO_SWITCH, (gu + eq));
}

void TEA6330T::writeToTEA6330T(uint8_t function, uint8_t val){
    _wire->beginTransmission(i2cAddress);

    _wire->write(function);
    _wire->endTransmission(false);

    _wire->write(val);
    _wire->endTransmission(true);
}

bool TEA6330T::init(){
    _wire->beginTransmission(i2cAddress);
    if(_wire->endTransmission()){
        return 0;
    }
    // Set default values
    volume_r = VOL_BASE;
    volume_l = VOL_BASE;
    fader_rear = FADER_REAR_OFF;
    fader_front = FADER_FRONT_OFF;
    bass_gain = BOT_BASE;
    treble_gain = BOT_BASE;

    fader_enabled = false;
    global_mute = false;
    equalizer_takeover = false;
    syncWithTEA6330T();
    return 1;
}

void TEA6330T::incrementVolume(int channel){
    uint8_t current_volume = (channel == TEA6330T_CHANNEL_L) ? volume_l : volume_r;
    uint8_t function = (channel == TEA6330T_CHANNEL_L) ? VOL_LEFT : VOL_RIGHT;
    if(current_volume != VOL_GAIN_MAX){
        uint8_t new_volume = current_volume++;
        if(new_volume != VOL_GAIN_MAX){
            writeToTEA6330T(function, new_volume);

            if(channel == TEA6330T_CHANNEL_L){
                volume_l = new_volume;
            }else{
                volume_r = new_volume;
            }
        }
    }
    
}

void TEA6330T::decrementVolume(int channel){
    uint8_t current_volume = (channel == TEA6330T_CHANNEL_L) ? volume_l : volume_r;
    uint8_t function = (channel == TEA6330T_CHANNEL_L) ? VOL_LEFT : VOL_RIGHT;
    if(current_volume != VOL_GAIN_MIN){
        uint8_t new_volume = current_volume--;
        if(new_volume != VOL_GAIN_MIN){
            writeToTEA6330T(function, new_volume);

            if(channel == TEA6330T_CHANNEL_L){
                volume_l = new_volume;
            }else{
                volume_r = new_volume;
            }
        }
    }
}

void TEA6330T::setVolume(int8_t val, int channel){
    uint8_t function = (channel == TEA6330T_CHANNEL_L) ? VOL_LEFT : VOL_RIGHT;
    uint8_t word = dbToWord(val);
    writeToTEA6330T(function, word);
    if(channel == TEA6330T_CHANNEL_L){
        volume_l = word;
    }else{
        volume_r = word;
    }
}

uint8_t TEA6330T::dbToWord(int dB, uint8_t function, uint8_t fader_channel = 1){

    int fromLow = 0;
    int fromHigh = 0;
    int toLow = 0;
    int toHigh = 0;

    switch(function){
        case VOL_LEFT:
        case VOL_RIGHT:
            fromLow = -66;
            fromHigh = 20;
            toLow = VOL_GAIN_MIN;
            toHigh = VOL_GAIN_MAX;
            break;
        case BASS:
            fromLow = -12;
            fromHigh = 15;
            toLow = BOT_GAIN_MIN;
            toHigh = BOT_GAIN_MAX;
            break;
        case TREBLE:
            fromLow = -12;
            fromHigh = 12;
            toLow = BOT_GAIN_MIN;
            toHigh = BOT_GAIN_MAX;
            break;
        case FADER:
            fromLow = -30;
            fromHigh = -2;
            if(fader_channel == TEA6330T_FADER_F){
                toLow = FADER_FRONT_MIN;
                toHigh = FADER_FRONT_MAX;
            }else{
                toLow = FADER_REAR_MIN;
                toHigh = FADER_REAR_MAX;
            }
            break;
        default:
        break;
    }
   return map(dB, fromLow, fromHigh, toLow, toHigh);
}

void TEA6330T::resetVolume(int channel = 0){
    if(channel == 0){
        setVolume(VOL_BASE, TEA6330T_CHANNEL_L);
        setVolume(VOL_BASE, TEA6330T_CHANNEL_R);
    }
    if(channel == TEA6330T_CHANNEL_L){
        setVolume(VOL_BASE, TEA6330T_CHANNEL_L);
    }
    if(channel == TEA6330T_CHANNEL_R){
        setVolume(VOL_BASE, TEA6330T_CHANNEL_R);
    }
}