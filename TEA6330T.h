#ifndef TEA6330T_H_
#define TEA6330T_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h>

#define TEA6330T_CHANNEL_L  1
#define TEA6330T_CHANNEL_R  2

#define TEA6330T_FADER_F  1
#define TEA6330T_FADER_R  2

// First 8-bits select function
typedef enum TEA6330T_SUBADDRESSES{
    VOL_LEFT        = 0b00000000, 
    VOL_RIGHT       = 0b00000001,
    BASS            = 0b00000010,
    TREBLE          = 0b00000011,
    FADER           = 0b00000100,
    AUDIO_SWITCH    = 0b00000101,
} subAddress;

typedef enum TEA6330T_VOLUMECONTROL{
    VOL_BASE            = 0b00110101, // No gain
    VOL_GAIN_MAX        = 0b00111111, // +20dB
    VOL_GAIN_MIN        = 0b00010100, // -66dB
    VOL_MUTE_CHANNEL    = 0b00000000, // One of 3 mute signals
} volControl;

// When external equalizer is enabled, treble controll doesn't work and bass control can only go in positive direction
typedef enum TEA6330T_BASS_OR_TREBLE_CONTROL{
    BOT_BASE            = 0b00000111, // No gain
    BOT_GAIN_MAX        = 0b00001111, // +15dB bass or +12dB treble (Those values are already reached at 0b00001100 without filtering)
    BOT_GAIN_MIN        = 0b00000000, // -12dB bass and treble (Those values are already reached at 0b00000011 without filtering)
} bassOrTrebleControl;

typedef enum TEA6330T_FADER_CONTROL{
    FADER_FRONT_OFF       = 0b00111111, // No gain
    FADER_REAR_OFF        = 0b00101111,

    FADER_FRONT_MAX       = 0b00111110, // -2dB
    FADER_FRONT_MIN       = 0b00110000, // -30dB
    FADER_FRONT_MUTE      = 0b00011110, // Mute front channel (-84dB)

    FADER_REAR_MAX       = 0b00101110, // -2dB
    FADER_REAR_MIN       = 0b00100000, // -30dB
    FADER_REAR_MUTE      = 0b00001110, // Mute rear channel (-84dB)
} faderControl;

typedef enum TEA6330T_AUDIO_SWITCH{
    GLOBAL_MUTE_ON          = 0b10000000,
    GLOBAL_MUTE_OFF         = 0b00000000,
    EQUALIZER_TAKEOVER_OFF  = 0b01000000,
    EQUALIZER_TAKEOVER_ON   = 0b00000000,
} audioSwitch;

class TEA6330T{
    public:
        // If your chip is failing to initialize, use i2c scanner script to determine if your address matches the default one used here. Otherwise check your wiring.
        bool init();
        // Default address is 64 (Or at least the chip i have had this address)
        TEA6330T(const int addr = 0x40) : _wire{&Wire}, i2cAddress{addr} {}
        TEA6330T(TwoWire *w, const int addr = 0x40) : _wire{w}, i2cAddress{addr} {}
        // Bump volume by 1 step in positive direction (increase volume)
        void incrementVolume(int channel);
        // Bump volume by 1 step in negative direction (decrease volume)
        void decrementVolume(int channel);
        // Set arbitrary volume in dB (working range is 20 <-> -66). Method accepts negative values. 
        void setVolume(int val, int channel);
        // Resets volume to default values (no gain) on both channels
        void resetVolume();
        // Set Bass gain (working range is 15 <-> -12). Method accepts negative values.
        void setBassGain(int val);
        // Set Treble gain (working range is 12 <-> -12). Method accepts negative values.
        void setTrebleGain(int val);
        // Resets bass and treble gain to default values (no gain)
        void resetEq();
        // Set fader gain (working range is -2 <-> -30) on either Front or Rear channel. Method accepts negative values.
        void setFaderGain(int val, int channel);
        // Resets fader channels gain to default values (no gain)
        void resetFader();
        // Mute front or rear channel 
        void muteFader(int channel);
        // Enable or disable global hardware mute
        void mute(bool mute);
        // Enable or disable the external equalizer takeover
        void letEqualizerTakeover(bool eq);
    protected:
        TwoWire *_wire;
        int i2cAddress;
        // Current operating values. Chip does not support reading, so these need to be set to default on each reset.
        uint8_t volume_r;
        uint8_t volume_l;
        uint8_t fader_rear;
        uint8_t fader_front;
        uint8_t bass_gain;
        uint8_t treble_gain;
        bool fader_enabled;
        bool global_mute;
        bool equalizer_takeover;
        // Writes single value at specified subaddress
        void writeToTEA6330T(uint8_t function, uint8_t val);
        // Writes all fields from this object to the chip
        void syncWithTEA6330T();
        // Translates an arbitrary dB value to 8-bit word that can be sent to the chip
        uint8_t dbToWord(int dB, uint8_t max, uint8_t min);
};
#endif