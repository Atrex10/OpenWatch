#ifndef MAINHEADER_H
#define MAINHEDER_H

// HARDWARE CONFIGURATION
#define USE_EXT_RTC
#define USE_BATTERY

// main arduino functions for src.ino
void appSetup();
void appLoop();

extern const bool debug;
extern const bool displayDots;
extern bool syncTimeFlag;

// PINS
extern const int thermistorADCPin;
extern const int thermistorEnablePin;

void setUpdateInterval(int interval);
void setRenderInterval(int interval);
void setButtonInterval(int interval);

void setSleepDelay(int delay, bool setDeafult=false);
void deepsleep();

extern const uint8_t numOfScreens;

int readBatteryVoltage();
int readBatteryLevel();

#endif
