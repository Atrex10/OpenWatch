#include "rtcFunc.h"

#include "DS3231.h"
#include "mainHeader.h"

bool h12 = false;
bool hPM = false;
bool CenturyBit = false;

DS3231 rtc;

int timeUpdateInterval = 1000;

timeSct curTime = { rtc.getSecond(), rtc.getMinute(), rtc.getHour(h12, hPM) };
dateSct curDate = { rtc.getDate(), rtc.getMonth(CenturyBit), rtc.getYear() };

// time reading functions
void updateCurTime() {
    curTime = { rtc.getSecond(), rtc.getMinute(), rtc.getHour(h12, hPM) };
}

void updateCurDate() {
    curDate = { rtc.getDate(), rtc.getMonth(CenturyBit), rtc.getYear() };
}

// time setting functions
void setCurTime(timeSct time) {
    rtc.setSecond(time.second);
    rtc.setMinute(time.minute);
    rtc.setHour(time.hour);
    if (debug) {
        Serial.println(time.second);
        Serial.println(time.minute);
        Serial.println(time.hour);}
}

void setCurDate(dateSct date) {
    rtc.setDate(date.day);
    rtc.setMonth(date.month);
    rtc.setYear(date.year);
    if(debug) {
        Serial.println(date.day);
        Serial.println(date.month);
        Serial.println(date.year);}
}
