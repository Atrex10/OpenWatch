# ESP32 based watch firmware for SH1106 OLED

![Main time screen on 1.3" blue oled display.](images/5b9b3871-3926-4629-af2a-6cfe58b1c459.jpg)

This is a simple firmware meant for **wrist watches** that provides regular digital watch functionality with ability to easly add sensors, feedback devices, and other functionality.

## Hardware
This is a list of hardware the code was tested on:
1. **MCU board:**\
    xiao esp32c6/esp32c3 super mini
2. **Display:**\
    monochrome 128x64 oled display
3. **RTC:**\
    DS3231
4. **Buttons:**
    - at least 3 tact switches connected to gpio specified in `buttonPins` in `main.cpp`.
    - at least 1 tact switch connected to gpio that is capable of waking the chip up from deepsleep (Depending on the board it can be labeled as RTC Pin or LP Pin or other) specified in `wakePins` in `main.cpp`.
5. **Battery:**\
    Any cell that supports li-po/li-ion charging. Not necessary, see next point if not used.
6. **Battery voltage divider:**
    - connections:
        ```
        GND -- R1 -- ADC -- R2 -- BAT+
        |_____C1______|
        ```

        ***R1/(R1+R2) = 2.0***\
        **C1 = 100nF**\
        The capacitor is only needed when using high resistance resistors (add it when the circuit does not work properly).\

        ADC is an analog pin specified by `batteryAdcPin` in `main.cpp`.\
        If you want to use other voltage divider factor, change the `batteryVoltDivFac` variable in `main.cpp`. Be careful not to exceed maximum ADC voltage (3.3V in most cases).

        If battery is not used, you should short ADC Pin to 3V3.

## Functionality
**Notice:** Buttons' indexes are specified by their pins order in `buttonPins` array. So button with id 0 is the first one specified in the array and so on.
### Screens navigation:
The deafult screen is clock screen. You can prett


## Build tips
### Arduino IDE uploading
This is the deafult build and upload option, and all the code is set to work with it. The `src.ini` file is **necessary** for this process, as it acts like a wrapper for `main.cpp` to work with Arduino IDE.

### PlatformIO uploading
If you have a board that **PlatformIO** supports, you can try this uploading option as it is **significantly faster**. There is a working `platformio.ini` file included in the repo configured for **esp32 c3** (tested on esp32 c3 super mini). You also have to edit the code a little bit:
1. Uncomment `// include <Arduino.h>` in `main.cpp`
2. Rename `appSetup` and `appLoop` functions to `setup` and `loop` correspondingly
3. It is recommended to delete `src.ini` from `/src/` directory to avoid compiling issues (although I didn't exprienced those with this file present)



## Customization
### Fonts
The fimware uses Adafruit's [SH110X](https://github.com/adafruit/Adafruit_SH110x) and [GFX](https://github.com/adafruit/Adafruit-GFX-Library) libraries for display. The **big clock numbers** use custom bitmaps stored in `clockNum.cpp`. You can replace those with your own bitmaps in the correct format (array of bytes). Be sure to match the size.\
\
The small text fonts are from Adafruit GFX and you can choose which one you want to use by editing `displayText()` function in `oled.cpp`.

### Icons
**Icons** (such as battery level and ':' or '.' symbols) can be very easly changed the same way as clock font, by editing their **bitmaps**. Notice that for example the `batteryIconBmps` array actualy consists of 6 individual arrays, each containing bitmaps for corresponding battery levels.
