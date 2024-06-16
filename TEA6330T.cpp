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
        writeToTEA6330T(VOL_LEFT, VOL_BASE);
        writeToTEA6330T(VOL_RIGHT, VOL_BASE);
        volume_l = VOL_BASE;
        volume_r = VOL_BASE;
    }
    if(channel == TEA6330T_CHANNEL_L){
        writeToTEA6330T(VOL_LEFT, VOL_BASE);
        volume_l = VOL_BASE;
    }
    if(channel == TEA6330T_CHANNEL_R){
        writeToTEA6330T(VOL_RIGHT, VOL_BASE);
        volume_r = VOL_BASE;
    }
}

void TEA6330T::setBassGain(int val){
    uint8_t gain = dbToWord(val, BASS);
    writeToTEA6330T(BASS, gain);
    bass_gain = gain;
}

void TEA6330T::setTrebleGain(int val){
    uint8_t gain = dbToWord(val, TREBLE);
    writeToTEA6330T(TREBLE, gain);
    treble_gain = gain;
}

void TEA6330T::resetEq(uint8_t channel = 0){
    if(channel == 0){
        writeToTEA6330T(BASS, BOT_BASE);
        writeToTEA6330T(TREBLE, BOT_BASE);
        bass_gain = BOT_BASE;
        treble_gain = BOT_BASE;
    }
    if(channel == BASS){
        writeToTEA6330T(BASS, BOT_BASE);
        bass_gain = BOT_BASE;
    }
    if(channel == TREBLE){
        writeToTEA6330T(TREBLE, BOT_BASE);
        treble_gain = BOT_BASE;
    }
}

void TEA6330T::setFaderGain(int val, int channel){
    uint8_t gain = dbToWord(val, FADER, channel);
    writeToTEA6330T(FADER, gain);
    if(channel == TEA6330T_FADER_F){
        fader_front = gain;
        fader_enabled = true;
    }
    if(channel == TEA6330T_FADER_R){
        fader_rear = gain;
        fader_enabled = true;
    }
}

void TEA6330T::resetFader(int channel = 0){
    if(channel == 0){
        writeToTEA6330T(FADER, FADER_FRONT_OFF);
        writeToTEA6330T(FADER, FADER_REAR_OFF);
        fader_front = FADER_FRONT_OFF;
        fader_rear = FADER_REAR_OFF;
        fader_enabled = false;
    }
    if(channel == TEA6330T_FADER_F){
        writeToTEA6330T(FADER, FADER_FRONT_OFF);
        fader_front = FADER_FRONT_OFF;
        fader_enabled = false;
    }
    if(channel == TEA6330T_FADER_R){
        writeToTEA6330T(FADER, FADER_REAR_OFF);
        fader_rear = FADER_REAR_OFF;
        fader_enabled = false;
    }
}

void TEA6330T::muteFader(int channel = 0){
    if(channel == 0){
        writeToTEA6330T(FADER, FADER_FRONT_MUTE);
        writeToTEA6330T(FADER, FADER_REAR_MUTE);
        fader_front = FADER_FRONT_MUTE;
        fader_rear = FADER_REAR_MUTE;
    }
    if(channel == TEA6330T_FADER_F){
        writeToTEA6330T(FADER, FADER_FRONT_MUTE);
        fader_front = FADER_FRONT_MUTE;
    }
    if(channel == TEA6330T_FADER_R){
        writeToTEA6330T(FADER, FADER_REAR_MUTE);
        fader_rear = FADER_REAR_MUTE;
    }
}

void TEA6330T::mute(bool mute){
    uint8_t eq_state = equalizer_takeover ? EQUALIZER_TAKEOVER_ON : EQUALIZER_TAKEOVER_OFF;
    if(mute){
        uint8_t word = GLOBAL_MUTE_ON + eq_state;
    }else{
        uint8_t word = GLOBAL_MUTE_OFF + eq_state;
    }
    writeToTEA6330T(AUDIO_SWITCH, word);
    global_mute = mute;
}

void TEA6330T::letEqualizerTakeover(bool eq){
    uint8_t mute_state = global_mute ? GLOBAL_MUTE_ON : GLOBAL_MUTE_OFF;
    if(eq){
        uint8_t word = EQUALIZER_TAKEOVER_ON + mute_state;
    }else{
        uint8_t word = EQUALIZER_TAKEOVER_OFF + mute_state;
    }
    writeToTEA6330T(AUDIO_SWITCH, word);
    equalizer_takeover = eq;
}