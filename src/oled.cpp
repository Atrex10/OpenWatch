#include <Adafruit_GFX.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "mainHeader.h"
#include "oled.h"
#include "clockNum.h"
#include "icons.h"

#ifndef USE_SSD1306_OLED
    #include <Adafruit_SH110X.h>
    Adafruit_SH1106G display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
#else
    #include <Adafruit_SSD1306.h>
    Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);
#endif

void sh1106_command(uint8_t cmd) {
    Wire.beginTransmission(DISPLAY_ADDR);
    Wire.write(0x00); // control byte: command
    Wire.write(cmd);
    Wire.endTransmission();
}

// screen segments start coordinates
const int segmentsCoords[7][2] = { {0, 0}, {72, 0}, {16, 56}, {80, 56}, {56, 0}, {0, 24}, {72, 24} };

void setDisplayContrast(uint8_t value) {
    // the contrast function only works for sh1106 oled displays
    #ifndef USE_SSD1306_OLED
        display.setContrast(value);
    #endif
}

void setDisplayRefresh(uint8_t value) {
    #ifndef USE_SSD1306_OLED
        sh1106_command(0xD5);

        uint8_t command = 0x00;

        switch (value)
        {
        case 0:
            command = 0x52;
            break;
        case 1:
            command = 0x51;
            break;
        case 2:
            command = 0x50;
            break;
        case 3:
            command = 0x60;
            break;
        case 4:
            command = 0xF0;
            break;
        
        default:
            break;
        }

        sh1106_command(command);
    #endif
}

// universal big clock numbers functions
void displayClockNum(int num, int x, int y, bool small) {
    if (!small) {
        display.drawBitmap(x, y, clockNumBmps[num], clockNumWidhts[num], clockNumHeights[num], 1);
    } else {
        display.drawBitmap(x, y, AODClockNumBmps[num], AODClockNumWidhts[num], AODClockNumHeights[num], 1);
    }
}

void displayClockNums(int nums, int x, int y, int spacing, bool small) {
    // the nums value HAS to be 2 or 3 digit for wanted behavior
    int num1 = 0;
    int num2 = 0;

    if (nums < 100) {
        num1 = nums / 10;
        num2 = nums % 10;
    } else if (nums < 1000) {
        num1 = nums / 100;
        num2 = nums % 100;
    }

    displayClockNum(num1, x, y, small);
    if (!small) {
        displayClockNum(num2, (x + clockNumWidhts[num1] + spacing), y, small);
    } else {
        displayClockNum(num2, (x + AODClockNumWidhts[num1] + spacing), y, small);
    }
}

void displayMiddleDot(bool doubleDot) {
    display.drawBitmap(segmentsCoords[4][0], segmentsCoords[4][1], dotsIconBmps[doubleDot ? 1 : 0], dotsIconWidth, dotsIconHeight, 1);
}

// universal text display functions
void displayText(String text, int x, int y, int fontsize) {
    display.setCursor(x, y+7); // 7 comes from font offset, you may have to adjust it for other fonts
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextSize(fontsize);
    display.setTextColor(1);
    display.print(text);
}

// icon specific funcitions
void displayBatIcon(int level, int x, int y) {
    display.drawBitmap(x, y, batteryIconBmps[level], batIconWidth, batIconHeight, 1);
}

void displayWiFiTIcon(int x, int y) {
    display.drawBitmap(x, y, WiFiTIconBmp, WiFiTIconWidth, WiFiTIconHeight, 1);
}
