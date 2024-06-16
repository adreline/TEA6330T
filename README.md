# TEA6330T
An Arduino library for the TEA6330T Sound fader control circuit. Written following the [TEA6330T datasheet](https://pdf1.alldatasheet.com/datasheet-pdf/view/19741/PHILIPS/TEA6330T.html).

## Usage

Install the library in Arduino IDE and include it in your sketch. Then initialize the object:

```cpp
#include <Wire.h>
#include "TEA6330T.h"
void setup() {
  Wire.begin();
  TEA6330T fader = TEA6330T();
  fader.init(); // Returns true on success and false on failure
}
```

Look at the example usage in the examples directory. Take a look at `TEA6330T.h` for available methods with their descriptions.

## Wiring the chip

The minimum wiring setup.

Connect chip pins 3 and 10 to the power supply's ground. Connect chip pin 18 to the power supply's VCC. Connect chip pins 11 and 12 to SCL and SDA on the arduino. Connect arduino GND pin to the power supply's ground. 

> Both analog and digital ground pins (3 and 10) need to be grounded for TEA6330T to work, otherwise it can get damaged.

This setup should allow for the chip to be discovered on the I2C lines via the I2C scanner script and for the commands to be sent to it.

## Disclaimer

I have not yet fully tested the library with a full setup including actual audio output.