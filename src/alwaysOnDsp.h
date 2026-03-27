#ifndef ALWAYS_ON_DSP_H
#define ALWAYS_ON_DSP_H

#include "Arduino.h"

extern RTC_DATA_ATTR bool AODEnabled;
void enableAOD();
void disableAOD();

void setAODWakeUpTimer();

void putOnAOD();

#endif
