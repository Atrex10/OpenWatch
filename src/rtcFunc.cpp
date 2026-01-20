#include "rtcFunc.h"

#include <DS3231.h>
#include <TimeLib.h>
#include <time.h>
#include <sys/time.h>
#include "mainHeader.h"

#ifdef USE_EXT_RTC
    bool h12 = false;
    bool hPM = false;
    bool CenturyBit = false;
#endif

#ifdef USE_EXT_RTC
    DS3231 rtc;
#endif

// presetting time and date to 00:00:00 01.01.2000
tmElements_t dateTime = {0, 0, 0, 0, 0, 0, 0};

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
// helper function
time_t getSystemTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}

void updateCurTime() {
    tmElements_t tempTime;

    breakTime(getSystemTimestamp(), tempTime);

    dateTime.Second = tempTime.Second;
    dateTime.Minute = tempTime.Minute;
    dateTime.Hour = tempTime.Hour;
}

void updateCurDate() {
    tmElements_t tempTime;

    breakTime(getSystemTimestamp(), tempTime);

    dateTime.Day = tempTime.Day;
    dateTime.Month = tempTime.Month;
    dateTime.Year = tempTime.Year - 30;  // converting from years since 1970 format to years since 2000
}

// time setting functions
void setCurTime(timeSct time) {
    // updating only time, date is the same as current
    tmElements_t tm;
    tm.Year = dateTime.Year + 30;  // timelib counts years from 1970, and this code from 2000
    tm.Month = dateTime.Month;
    tm.Day = dateTime.Day;
    tm.Hour = time.hour;
    tm.Minute = time.minute;
    tm.Second = time.second;

    time_t t = makeTime(tm); 

    // set ESP system time
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    updateCurTime();

    if (debug) {
        Serial.println(time.second);
        Serial.println(time.minute);
        Serial.println(time.hour);}
}

void setCurDate(dateSct date) {
    // updating only date, time is the same as current
    tmElements_t tm;
    tm.Year = date.year + 30;  // timelib counts years from 1970, and this code from 2000
    tm.Month = date.month;
    tm.Day = date.day;
    tm.Hour = dateTime.Hour;
    tm.Minute = dateTime.Minute;
    tm.Second = dateTime.Second;

    time_t t = makeTime(tm); 

    // set ESP system time
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, NULL);

    updateCurDate();

    if(debug) {
        Serial.println(date.day);
        Serial.println(date.month);
        Serial.println(date.year);}
}

#endif
