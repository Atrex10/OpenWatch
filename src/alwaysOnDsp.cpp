#include "alwaysOnDsp.h"
#include "rtcFunc.h"
#include "oled.h"

#include "Arduino.h"

RTC_DATA_ATTR bool AODEnabled = false;

const int AODClockSegments[4][2][2] = { {{0, 0}, {44, 0}}, 
                                        {{40, 0}, {84, 0}}, 
                                        {{0, 32}, {44, 32}}, 
                                        {{40, 32}, {84, 32}}};
const int numsSpacing = 4;

const int AODDisplayRefresh = 0;
const int AODDisplayContrast = 0;

void enableAOD() {
    AODEnabled = true;
}

void disableAOD() {
    AODEnabled = false;
}

void setAODWakeUpTimer() {
    uint64_t microsBeforeWakeUp = 1000000ULL * (60 - dateTime.Second + 1);  // add 1 second to avoid waking up before minute passed

    esp_sleep_enable_timer_wakeup(microsBeforeWakeUp);
}

void putOnAOD() {
    display.clearDisplay();
    
    int currentSegment = dateTime.Minute % 4;

    displayClockNums(dateTime.Hour, 
                    AODClockSegments[currentSegment][0][0], 
                    AODClockSegments[currentSegment][0][1],
                    numsSpacing, true);
    
    displayClockNums(dateTime.Minute, 
        AODClockSegments[currentSegment][1][0], 
        AODClockSegments[currentSegment][1][1],
        numsSpacing, true);

    // displayClockNums(dateTime.Second, 10, 10, 6);

    // set display settings
    setDisplayRefresh(AODDisplayRefresh);
    setDisplayContrast(AODDisplayContrast);

    display.display();
}
