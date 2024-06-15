#include <Wire.h>

// First 8-bits select function
typedef enum TEA6330T_SUBADDRESSES{
    VOL_LEFT        = 0b00000000, 
    VOL_RIGHT       = 0b00000001,
    BASS            = 0b00000010,
    TREBLE          = 0b00000011,
    FADER           = 0b00000100,
    AUDIO_SWITCH    = 0b00000101,
} subAddress;

typedef enum TEA6330T_VOLUMECONTROLL{
    BASE            = 0b00110101, // No gain
    GAIN_MAX        = 0b00111111, // +20dB
    GAIN_MIN        = 0b00010100, // -66dB
    MUTE_CHANNEL    = 0b00000000, // One of 3 mute signals
} volControll;

// When external equalizer is enabled, treble controll doesn't work and bass controll can only go in positive direction
typedef enum TEA6330T_BASS_OR_TREBLE_CONTROLL{
    BASE            = 0b00000111, // No gain
    GAIN_MAX        = 0b00001111, // +15dB bass or +12dB treble (Those values are already reached at 0b00001100 without filtering)
    GAIN_MIN        = 0b00000000, // -12dB bass and treble (Those values are already reached at 0b00000011 without filtering)
} bassOrTrebleControll;

typedef enum TEA6330T_FADER_CONTROLL{
    FRONT_OFF       = 0b00111111, // No gain
    REAR_OFF        = 0b00101111,

    FRONT_MAX       = 0b00111110, // -2dB
    FRONT_MIN       = 0b00110000, // -30dB
    FRONT_MUTE      = 0b00011110, // Mute front channel (-84dB)

    REAR_MAX       = 0b00101110, // -2dB
    REAR_MIN       = 0b00100000, // -30dB
    REAR_MUTE      = 0b00001110, // Mute rear channel (-84dB)
} faderControll;

typedef enum TEA6330T_AUDIO_SWITCH{
    GLOBAL_MUTE_ON          = 0b10000000,
    GLOBAL_MUTE_OFF         = 0b00000000,
    EQUALIZER_TAKEOVER_OFF  = 0b01000000,
    EQUALIZER_TAKEOVER_ON   = 0b00000000,
}