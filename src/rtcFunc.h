#ifndef RTCFUNC_H
#define RTCFUNC_H

#include <DS3231.h>
#include <TimeLib.h>

struct timeSct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
};

struct dateSct {
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

// extern bool h12; // something for rtc
// extern bool hPM; // something for rtc
// extern bool CenturyBit; // something for rtc

extern int timeUpdateInterval; // in ms, how often ticker updates current time

extern DS3231 rtc;
extern tmElements_t dateTime;

// extern timeSct curTime;
// extern dateSct curDate;

void updateCurTime();
void updateCurDate();

void setCurTime(timeSct time);
void setCurDate(dateSct date);

#endif
