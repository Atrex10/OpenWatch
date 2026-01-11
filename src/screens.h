#ifndef SCREENS_H
#define SCREENS_H

#include "Screen.h"

#include "oled.h"
#include "mainHeader.h"
#include "rtcFunc.h"
#include "sensors.h"

// ________DUMMY________
class DummyScreen : public Screen {
    public:
        DummyScreen() {}

        void onEnter() override {
            setUpdateInterval(1000);
            setRenderInterval(1000);
            setSleepDelay(10);
            update(1000);
            render();
        }

        void onExit() override {
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            ;
        }

        void render() override {
            display.clearDisplay();
            
            displayText("dummy", 0, 0);

            display.display();
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return -1;
                break;
            
            case 1:
                return -2;
                break;
            
            default: return -1; break; }
        }
};

// ________CLOCK SCREEN________
class ClockScreen : public Screen {
    public:
        ClockScreen() {}

        int hours;
        int minutes;
        int seconds;
        int batLevel;

        void onEnter() override {
            setUpdateInterval(1000);
            setRenderInterval(1000);
            setSleepDelay(10);
            update(1000);
            render();
        }

        void onExit() override {
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            hours = curTime.hour;
            minutes = curTime.minute;
            seconds = curTime.second;

            batLevel = readBatteryLevel();
        }

        void render() override {
            display.clearDisplay();
            
            displayClockNums(hours, segmentsCoords[0][0], segmentsCoords[0][1]); // drawing hours in segment 0
            displayClockNums(minutes, segmentsCoords[1][0], segmentsCoords[1][1]); // drawing minutes in segment 1
            if (displayDots) displayMiddleDot(true); // double dots in the middle

            displayBatIcon(batLevel, segmentsCoords[2][0], segmentsCoords[2][1]); // drawing battery icon in segment 2
            displayText(String(seconds), segmentsCoords[3][0], segmentsCoords[3][1]); // drawing seconds in segment 3

            display.display();
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return -1;
                break;
            
            case 1:
                return -2;
                break;
            
            case 2:
                return 2; // date screen
                break;
            
            default: return -1; break; }
        }
};

// ________CLOCK SCREEN________
class DateScreen : public Screen {
    public:
        DateScreen() {}

        int year;
        int month;
        int day;
        int batVolt;

        void onEnter() override {
            setUpdateInterval(1000);
            setRenderInterval(1000);
            setSleepDelay(10);
            update(1000);
            render();
        }

        void onExit() override {
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            day = curDate.day;
            month = curDate.month;
            year = curDate.year;

            batVolt = readBatteryVoltage();
        }

        void render() override {
            display.clearDisplay();
            
            displayClockNums(day, segmentsCoords[0][0], segmentsCoords[0][1]); // drawing day in segment 0
            displayClockNums(month, segmentsCoords[1][0], segmentsCoords[1][1]); // drawing month in segment 1

            if (displayDots) displayMiddleDot();

            displayText((String(batVolt) + "mV"), segmentsCoords[2][0]-16, segmentsCoords[2][1]); // drawing battery voltage in segment 2
            displayText(String(2000 + year), segmentsCoords[3][0], segmentsCoords[3][1]); // drawing year in segment 3

            display.display();
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return 0;
                break;
            
            case 1:
                return -2;
                break;
            
            default: return -1; break; }
        }
};

// ________STOPWATCH________
class StopwatchScreen : public Screen {
    public:
        StopwatchScreen() {}

        bool hourMinForm;

        bool running;
        bool rendering;

        uint16_t elapsedMillis;
        uint8_t elapsedSeconds;
        uint8_t elapsedMinutes;
        uint8_t elapsedHours;

        void onEnter() override {
            stopStopwatch();
            resetStopwatch();

            setUpdateInterval(10);
            
            render();
        }

        void onExit() override {
            setSleepDelay(0, true);
            
            display.clearDisplay();
            display.display();
        }
        
        // stopwatch logic
        bool secondsClicked; // flag that tells if seconds clicked, but for stopwatch, not rtc
        uint32_t lastMillis;
        uint8_t lastSeconds;
        uint16_t lastSecClickMillis = 0;
        int16_t secClickMillisOffset;
        
        void startStopwatch() {
            lastMillis = millis();
            lastSeconds = curTime.second;
            secondsClicked = true;

            secClickMillisOffset = lastSecClickMillis;
            // this part is for resuming stopwatch so it doesnt count millis over 1000
            secClickMillisOffset -= elapsedMillis;
            if (secClickMillisOffset < 0) {
                secClickMillisOffset += 1000;
                secondsClicked = false; }
            
            running = true;
            rendering = true;

            setSleepDelay(0); // disabling auto deepsleep
            setRenderInterval(50);
        }

        void stopStopwatch() {
            running = false;

            setRenderInterval(1000);
        }

        void resetStopwatch() {
            hourMinForm = false;
            running = false;
            rendering = true;
            secondsClicked = true;
            elapsedMillis = 0;
            elapsedSeconds = 0;
            elapsedMinutes = 0;
            elapsedHours = 0;

            setSleepDelay(180); // 3 min delay
        }

        void update(int dt) override {
            if (debug) {
                Serial.print("running: ");
                Serial.println(running); }
            
            uint8_t newSeconds = curTime.second;
            uint8_t newMinutes = curTime.minute;
            uint8_t newHours = curTime.hour;

            if (lastSeconds != newSeconds) {
                lastSecClickMillis = 0;
                secondsClicked = false;
            } else {
                lastSecClickMillis += dt;
            }

            // updating last time values
            lastSeconds = newSeconds;
            
            if (!running) {return;}

            if (lastSecClickMillis >= secClickMillisOffset && !secondsClicked) {
                secondsClicked = true;
                elapsedSeconds++;
                elapsedMillis = 0;
            } else {
                elapsedMillis += dt;
            }

            // controlling second, minute and hour overflow
            if (elapsedSeconds >= 60) {
                elapsedSeconds -= 60;
                elapsedMinutes++;
            }
            if (elapsedMinutes >= 60) {
                if (!hourMinForm) {
                    hourMinForm = true;
                    setUpdateInterval(1000);
                    setRenderInterval(1000); }
                elapsedMinutes -= 60;
                elapsedHours++;
            }
            if (elapsedHours >= 99) {
                stopStopwatch();
            }
        }

        void render() override {
            if (debug) {
                Serial.print("rendering: ");
                Serial.println(rendering); }
            if (rendering) {
                display.clearDisplay();

                if (!hourMinForm) {
                    displayClockNums(elapsedMinutes, segmentsCoords[0][0], segmentsCoords[0][1]);
                    displayClockNums(elapsedSeconds, segmentsCoords[1][0], segmentsCoords[1][1]);
                    displayText("M:S", segmentsCoords[2][0], segmentsCoords[2][1]);
                    displayText(String(elapsedMillis), segmentsCoords[3][0], segmentsCoords[3][1]);
                } else {
                    displayClockNums(elapsedHours, segmentsCoords[0][0], segmentsCoords[0][1]);
                    displayClockNums(elapsedMinutes, segmentsCoords[1][0], segmentsCoords[1][1]);
                    displayText("H:M:s", segmentsCoords[2][0], segmentsCoords[2][1]);
                    displayText(String(elapsedSeconds), segmentsCoords[3][0], segmentsCoords[3][1]);
                }
                if (displayDots) displayMiddleDot(true); // double dots in the middle

                display.display();
            }
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return 0;
                break;
            
            case 1:
                return -2;
                break;
            
            case 2:
                if (running) {
                    rendering = !rendering;
                } else {
                    resetStopwatch();
                }
                return -1;
                break;

            case 3:
                if (running) {
                    stopStopwatch();
                } else {
                    startStopwatch();
                }
                return -1;
                break;
            
            default: return -1; break; }
        }
};

// ________TEMPERATURE________
class TemperatureScreen : public Screen {
    public:
        TemperatureScreen() {}

        float temperature = 0;
        bool negative;

        int displayTemperature = 0;
        int nums1;
        int nums2;

        void onEnter() override {
            setUpdateInterval(1000);
            setRenderInterval(1000);
            setSleepDelay(10);

            enableThermistor();

            update(1000);
            render();
        }

        void onExit() override {
            disableThermistor();
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            temperature = tempRead();

            if (debug) {
                Serial.println(temperature); }

            displayTemperature = abs(100 * temperature);
            nums1 = displayTemperature / 100;
            nums2 = displayTemperature % 100;

            negative = temperature < 0;
        }

        void render() override {
            display.clearDisplay();

            displayClockNums(nums1, segmentsCoords[0][0], segmentsCoords[0][1]);
            displayClockNums(nums2, segmentsCoords[1][0], segmentsCoords[1][1]);
            displayMiddleDot();

            displayText("Temp.", segmentsCoords[2][0], segmentsCoords[2][1]);
            displayText(String(negative ? "-" : "") + "*C", segmentsCoords[3][0], segmentsCoords[3][1]);

            display.display();
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return 0;
                break;
            
            case 1:
                return -2;
                break;
            
            default: return -1; break; }
        }
};

// ________SETTINGS SCREEN________
// (unused, adapt when there will be need to)
class SettingsScreen : public Screen {
    public:
        SettingsScreen() {}

        // settings values (unused for now)
        uint8_t currentSettingId = 0;
        
        void onEnter() override {
            setUpdateInterval(1000);
            setRenderInterval(100);
            setSleepDelay(45);
            update(1000);
            render();
        }

        void onExit() override {
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            ;
        }

        void render() {
            ;
        }
        
        int handleInput(int button) override {
            switch (button) {
            case 0:
                return 0;
                break;
              
            case 1:
                return -2;
                break;
            
            default: return -1; break; }
        }
};

// ________WIFITIME________
class WiFiTimeScreen : public Screen {
    public:
        WiFiTimeScreen() {}

        bool active;
        bool connectionStatus;
        bool synchronizationStatus;

        void onEnter() override {
            setUpdateInterval(200);
            setRenderInterval(1000);
            setSleepDelay(45);
            
            active = false;
            connectionStatus = false;
            synchronizationStatus = false;

            render();
        }

        void onExit() override {
            display.clearDisplay();
            display.display();
        }

        void update(int dt) override {
            ;
        }

        void render() override {
            display.clearDisplay();
            
            displayText("WiFiT", segmentsCoords[3][0] - 16, segmentsCoords[3][1]);
            displayText((active ? "Act" : "Inact"), segmentsCoords[2][0] - 16, segmentsCoords[2][1]);

            displayText("Con", segmentsCoords[0][0], segmentsCoords[0][1]+8);
            displayText(connectionStatus ? "Yes" : "No", segmentsCoords[5][0], segmentsCoords[5][1]);

            displayText("Sync", segmentsCoords[1][0], segmentsCoords[1][1]+8);
            displayText(synchronizationStatus ? "Yes" : "No", segmentsCoords[6][0], segmentsCoords[6][1]);

            display.display();
        }

        int handleInput(int button) override {
            switch (button) {
            case 0:
                return 0;
                break;
            
            case 1:
                return -2;
                break;
            
            case 3:
                syncTimeFlag = true;
                deepsleep();
                return -1;
                break;
            
            default: return -1; break; }
        }
};

#endif
