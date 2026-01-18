#include "rtcFunc.h"

#include <DS3231.h>
#include <TimeLib.h>
#include <ESP32Time.h>
#include "mainHeader.h"

// #define USE_EXT_RTC

#ifdef USE_EXT_RTC
    bool h12 = false;
    bool hPM = false;
    bool CenturyBit = false;
#endif

#ifdef USE_EXT_RTC
    DS3231 rtc;
#else
    ESP32Time rtc;
#endif

tmElements_t dateTime;

int timeUpdateInterval = 1000;

// timeSct curTime = { rtc.getSecond(), rtc.getMinute(), rtc.getHour(h12, hPM) };
// dateSct curDate = { rtc.getDate(), rtc.getMonth(CenturyBit), rtc.getYear() };

// time reading functions for external rtc
#ifdef USE_EXT_RTC
void updateCurTime() {
    dateTime.Second = rtc.getSecond();
    dateTime.Minute = rtc.getMinute();
    dateTime.Hour = rtc.getHour(h12, hPM);
}

void updateCurDate() {
    dateTime.Day = rtc.getDate();
    dateTime.Month = rtc.getMonth(CenturyBit);
    dateTime.Year = rtc.getYear();
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

#else
void updateCurTime() {
    dateTime.Second = rtc.getSecond();
    dateTime.Minute = rtc.getMinute();
    dateTime.Hour = rtc.getHour(true);  // true for 24h format
}

void updateCurDate() {
    dateTime.Day = rtc.getDay();
    dateTime.Month = rtc.getMonth() + 1;  // adding one to account for 0-11 month format
    dateTime.Year = rtc.getYear();
}

// time setting functions
void setCurTime(timeSct time) {
    rtc.setTime(time.second, time.minute, time.hour, dateTime.Day, dateTime.Month, dateTime.Year);  // only updating time
    if (debug) {
        Serial.println(time.second);
        Serial.println(time.minute);
        Serial.println(time.hour);}
}

void setCurDate(dateSct date) {
    rtc.setTime(dateTime.Second, dateTime.Minute, dateTime.Hour, date.day, date.month, date.year);  // only updating date
    if(debug) {
        Serial.println(date.day);
        Serial.println(date.month);
        Serial.println(date.year);}
}

#endif
