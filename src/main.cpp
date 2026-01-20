// This code was written to be used on esp32 c3, but then i switched to xiao esp32 c6.
// Since platformio does not support that board, it is necessary to build it with Arduino IDE.
// The src.ino file is just a wrapper and there is actual code there.
// appSetup and appLoop functions are executed in their corresponding places in src.ino.
// If you want to build with platformio, then it is recommended to remove src.ino from /src folder and uncommend #include <Arduino.h>.
// platformio.ini file is set to work with esp32 c3.

// #include <Arduino.h>
#include <Wire.h>
#include <DS3231.h>
#include <Ticker.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_bt.h>

#include "mainHeader.h"
#include "oled.h"
#include "rtcFunc.h"
#include "WiFiTime.h"
#include "sensors.h"
#include "screens.h"
#include "screenMgr.h"

// ============ IMPORTANT VARIABLES ============
// =============================================
const bool debug = false; // debug flag, starts serial and prints info there
const bool displayDots = true;

// buttons variables
const int wakePins[2] = {0, 2};
const int buttonPins[4] = {0, 19, 20, 18};
bool buttonStates[4] = {false, false, false, false};

const int batteryAdcPin = 1;

const int thermistorADCPin = 2;
const int thermistorEnablePin = 16;

const uint8_t displayContrast = 0;

RTC_DATA_ATTR bool syncTimeFlag = false; // part of not logical, but the only working WiFiTime code
bool touchWakeFlag = false;

const int touchWakeSleepDelay = 4;
const int normalWakeSleepDelay = 6;

const int cpuSpeed = 80;  // MHz, for esp32 c6 deafult is 160; valid options: 240, 160, 80, 40

// ============ TIME UPDATES THINGS ============
// =============================================
void updateTimeAndDate() {
  updateCurTime();
  updateCurDate();
}

Ticker timeUpdateTicker;

// ============ DEEPSLEEP HANDLING ============
// ============================================
const int deepsleepTickerInterval = 1000; // in ms, how often the code checks if it is time to go into deepsleep mode
int sleepDelaySec = 10; // defines how much time from activity has to elapse for the esp to go into deepsleep mode; set to 0 to disable auto deepsleep
const int deafultSleepDelaySec = 10;
unsigned long lastActivityMillis = 0;

void setSleepDelay(int delay, bool setDeafult) {
  if (setDeafult) {
    sleepDelaySec = deafultSleepDelaySec;
  } else {
   sleepDelaySec = delay; }
}

void deepsleep(){
  // clearing the oled display so it doesnt consume power
  display.clearDisplay();
  display.display();

  // disabling Thermistor
  disableThermistor();
  delay(100);

  esp_deep_sleep_enable_gpio_wakeup(1ULL <<wakePins[0], ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_enable_gpio_wakeup(1ULL <<wakePins[1], ESP_GPIO_WAKEUP_GPIO_LOW);
  esp_deep_sleep_start();
}

void autoDeepsleepCheck() {
  if (sleepDelaySec) {
    if ((millis() - lastActivityMillis) >= (sleepDelaySec * 1000)) {
      if (debug) {
        Serial.print("Going to sleep after ");
        Serial.print(sleepDelaySec);
        Serial.println("s of inactivity"); }

      deepsleep();
    }
  }
}

Ticker deepsleepTicker;


// ============ BUTTONS FUNCTIONALITY HANDLING ============
// ========================================================
int buttonDebounceMs = 20; // how much time you have to wait before another press; filters out debouncing of tact switches

void handleBut0(){buttonStates[0] = true; lastActivityMillis = millis();}
void handleBut1(){buttonStates[1] = true; lastActivityMillis = millis();}
void handleBut2(){buttonStates[2] = true; lastActivityMillis = millis();}
void handleBut3(){buttonStates[3] = true; lastActivityMillis = millis();}


bool buttonPressed(int id) {
  if (buttonStates[id]) {
    buttonStates[id] = false;
    // if ((millis() - lastActivityMillis) >= buttonCooldownMs) {
    //   Serial.println("Press debounced");
    //   return true;
    // }
    delay(buttonDebounceMs);
    return (digitalRead(buttonPins[id]) == LOW);
  }
  return false;
}


// ============ OTHER ============
// ===============================
// adc with voltage divider reading
float readAdcVoltDiv(int pin, float factor) {
  #ifdef USE_BATTERY
    // reads voltage on specified pin and multiplies the value by given factor to compensate for voltage divider
    return factor * analogReadMilliVolts(pin);
  #else
    return 5000;  // always display full battery
  #endif
}

// battery level reading
const float batteryVoltDivFac = 2.0;
const int criticalBatteryVoltage = 3100;
const int batteryLevelsThresholds[6] = { 4100, 3900, 3700, 3500, 3200, criticalBatteryVoltage };

int readBatteryVoltage() {
  return readAdcVoltDiv(batteryAdcPin, batteryVoltDivFac);
}

int readBatteryLevel() {
  int voltage = readBatteryVoltage();
  if (debug) {
    Serial.println(voltage);
  }

  if (voltage > batteryLevelsThresholds[0]) {
    return 5;
  } else if (voltage >= batteryLevelsThresholds[1]){
    return 4;
  } else if (voltage >= batteryLevelsThresholds[2]){
    return 3;
  } else if (voltage >= batteryLevelsThresholds[3]){
    return 2;
  } else if (voltage >= batteryLevelsThresholds[4]){
    return 1;
  } else if (voltage >= batteryLevelsThresholds[5]){
    return 0;
  } else {
    delay(1000);
    deepsleep();
  }
}

void checkLowBattery() {
  int voltage = readBatteryVoltage();

  if (voltage < criticalBatteryVoltage) {
    deepsleep(); // goes to deepsleep to consume least power possible
    }
}

Ticker batteryControlTicker; // this ticker reads battery level to shut down the device if battery is critical low

// ============ SCREEN MANAGER  ============
// =========================================
int buttonCheckInterval = 10; // in ms, button checker interval, not effective when lower than 10

int updateInterval = 1000; // in ms, screen update interval, set by each screen inividualy, not effective when lower than 10
int renderInterval = 1000;// in ms, screen render interval, set by each screen inividualy, not effective when lower than 10

// creating screens instances
ClockScreen scrClock;
StopwatchScreen scrStopwatch;
TimerScreen scrTimer;
DateScreen scrDate;
WiFiTimeScreen scrWiFiT;

const uint8_t numOfScreens = 5;
Screen* screens[numOfScreens] = {
  &scrClock,
  &scrStopwatch,
  &scrDate,
  &scrTimer,
  &scrWiFiT,
};

ScreenManager manager(screens, numOfScreens);

// MANAGER UPDATE TICKER
unsigned long lastUpdateMillis = 0;
unsigned long lastRenderMillis = 0;
unsigned long lastButCheckMillis = 0;

void updateScrManager() {
  int dt = millis() - lastUpdateMillis;
  if (dt >= updateInterval) {
    manager.update(dt);
    lastUpdateMillis = millis();
  }
}
Ticker scrMgrUpdateTicker;

void renderScrManager() {
  if ((millis() - lastRenderMillis) >= renderInterval) {
    manager.render();
    lastRenderMillis = millis();
  }
}
Ticker scrMgrRenderTicker;

void checkButtons() {
  if ((millis() - lastButCheckMillis) >= buttonCheckInterval) {
    for (int i=0; i<4; i++) {
      if (buttonPressed(i)) {
        if (debug) {
          Serial.print("button pressed: ");
          Serial.println(i);}
        manager.handleInput(i);
        delayMicroseconds(10);
      }
    }
    lastButCheckMillis = millis();
  }
}
Ticker buttonCheckerTicker;

// intervals changing functions
void setUpdateInterval(int interval) {
  updateInterval = interval;
}

void setRenderInterval(int interval) {
  renderInterval = interval;
}

void setButtonInterval(int interval) {
  buttonCheckInterval = interval;
}

// ============ STANDARD ARDUINO FUNCTIONS ============
// ====================================================
void appSetup() {
  // hardware settings
  setCpuFrequencyMhz(cpuSpeed);

  // updating deepsleep activity counter (not important for boot, only for waking up)
  lastActivityMillis = millis();

  esp_sleep_wakeup_cause_t wakeCcause = esp_sleep_get_wakeup_cause();

  if (wakeCcause == ESP_SLEEP_WAKEUP_GPIO) {
    uint64_t wakeUpPins = esp_sleep_get_gpio_wakeup_status();

    if (wakeUpPins & (1ULL << wakePins[1])) {
      touchWakeFlag = true;  // woke up with touch sensor
    }
  }

  // initiating serial for debug
  if (debug) {
    Serial.begin(115200);
    delay(200);
    Serial.println("Serial intialized corectly"); }

  // tickers
  deepsleepTicker.attach_ms(deepsleepTickerInterval, autoDeepsleepCheck);
  timeUpdateTicker.attach_ms(timeUpdateInterval, updateTimeAndDate);

  // tickers run every 10ms, but they only update/render/check with the set interval
  scrMgrUpdateTicker.attach_ms(10, updateScrManager);
  scrMgrRenderTicker.attach_ms(10, renderScrManager);
  buttonCheckerTicker.attach_ms(10, checkButtons);
  batteryControlTicker.attach(10, checkLowBattery);

  // setting ADC to 12 bit resolution
  analogReadResolution(12);

  // initiating pins
  pinMode(wakePins[0], INPUT_PULLUP);
  pinMode(wakePins[1], INPUT_PULLUP);

  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(buttonPins[2], INPUT_PULLUP);
  pinMode(buttonPins[3], INPUT_PULLUP);

  pinMode(batteryAdcPin, INPUT);

  // pinMode(thermistorADCPin, INPUT);
  // pinMode(thermistorEnablePin, OUTPUT);
  // digitalWrite(thermistorEnablePin, LOW);  // disabling thermistor voltage divider

  // initiating interrupts
  attachInterrupt(digitalPinToInterrupt(buttonPins[0]), handleBut0, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[1]), handleBut1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[2]), handleBut2, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPins[3]), handleBut3, FALLING);
  
  // initiating wire
  Wire.begin();

  // initiating oled
  display.begin(0x3C, true);
  display.clearDisplay();
  display.display();
  // setting display brightness to predefined value
  setDisplayContrast(displayContrast);

  // updating time and date just to make sure they are fully up to date
  updateTimeAndDate();
  manager.setScreen(0);  // home screen
  
  if (touchWakeFlag) {
    setSleepDelay(touchWakeSleepDelay);  // small sleep delay if only turned on and not used further
  } else {
    setSleepDelay(normalWakeSleepDelay);
  }

  // syncinc time through WiFi if syncTimeFlag is set
  if (syncTimeFlag) {
    syncTimeFlag = false; // resetting flag

    // finding WiFiT screen index in screens
    int WiFiTScrIndex;
    for (int i=0; i < sizeof(screens); i++) {
      if (screens[i] == &scrWiFiT) {
        WiFiTScrIndex = i;
        break; }}

    manager.setScreen(WiFiTScrIndex); // setting screen
    scrWiFiT.onEnter(); // using WiFiT screen enter settings
    scrWiFiT.active = true;
    scrWiFiT.render();

    scrWiFiT.connectionStatus = connectAnyWiFi();
    WiFi.setSleep(false);
    scrWiFiT.render();
    if (scrWiFiT.connectionStatus) {
      scrWiFiT.synchronizationStatus = syncWiFiTime(); }
    scrWiFiT.active = false;
    scrWiFiT.render();

    WiFi.setSleep(true);
    disableWiFi();
  }

  // disabling wifi, ble & serial (if not debugging) after (maybe) syncing clock
  esp_wifi_stop();
  esp_wifi_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
  if (!debug) {
    Serial.end(); }
}

void appLoop() {
  // nothing here, everything is done through tickers
}
