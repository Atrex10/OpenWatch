#ifndef MAINHEADER_H
#define MAINHEADER_H

// HARDWARE CONFIGURATION
#define USE_EXT_RTC
#define USE_BATTERY
// #define USE_SSD1306_OLED
#define USE_BUZZER_ALARM

// main arduino functions for src.ino
void appSetup();
void appLoop();

// other
extern const bool debug;
extern const bool displayDots;
extern bool syncTimeFlag;

extern const int thermistorADCPin;
extern const int thermistorEnablePin;

void setUpdateInterval(int interval);
void setRenderInterval(int interval);
void setButtonInterval(int interval);

void setSleepDelay(int delay, bool setDeafult=false);
void deepsleep();

extern const uint8_t numOfScreens;

void playBuzzer(int freq, int duration);
extern const uint16_t buzzerFrequency;
extern const uint16_t buzzerBeepDuration;
extern const uint16_t buzzerAlarmDuration;

int readBatteryVoltage();
int readBatteryLevel();

#endif
