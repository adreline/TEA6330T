#include "TEA6330T.h"

void TEA6330T::syncWithTEA6330T(){
    writeToTEA6330T(VOL_LEFT, volume_l);
    writeToTEA6330T(VOL_RIGHT, volume_r);
    writeToTEA6330T(BASS, bass_gain);
    writeToTEA6330T(TREBLE, treble_gain);
    writeToTEA6330T(FADER, fader_rear);
    writeToTEA6330T(FADER, fader_front);
    // TODO
    //writeToTEA6330T(AUDIO_SWITCH, fader_front);
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