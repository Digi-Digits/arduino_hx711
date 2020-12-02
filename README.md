
Sketch that implements interface with [HX711 24-Bit ADC module](https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf). Prints the raw sensor read to the serial monitor (baud 112500).

The modules runs on 2.6 - 5.5 volts.

Change the gain with on line 16:

```
//  Table 3 Input Channel and Gain Selection
//  gain  factor  range
//  1     128     20mV
//  3     64      40mV
//  2     32      80mV
const uint8_t hx711_gain = 1;
```
Wiring:

    Arduino -----   Module
    D2      -----   SCK (PD_SCK)
    D3      -----   DT (DOUT)


The hx711 module can be used with bar load cells. Setup the connections like this diagram.

![first_launch](/wiring.jpg)


Tested on Arduino Uno (atmega328p)
