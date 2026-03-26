#include "alwaysOnDsp.h"
#include "rtcFunc.h"
#include "oled.h"

bool AODEnabled = true;

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
    // TODO: write layout
    displayClockNums(dateTime.Second, 10, 10, 6);

    display.display();
}
