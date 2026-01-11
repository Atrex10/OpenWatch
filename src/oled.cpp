#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "oled.h"
#include "clockNum.h"
#include "icons.h"

Adafruit_SH1106G display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, -1);

// screen segments start coordinates
const int segmentsCoords[7][2] = { {0, 0}, {72, 0}, {16, 56}, {80, 56}, {56, 0}, {0, 24}, {72, 24} };

void setDisplayContrast(uint8_t value) {
    display.setContrast(value);
}

// universal big clock numbers functions
void displayClockNum(int num, int x, int y) {
    display.drawBitmap(x, y, clockNumBmps[num], clockNumWidhts[num], clockNumHeights[num], 1);
}

void displayClockNums(int nums, int x, int y, int spacing) {
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

    displayClockNum(num1, x, y);
    displayClockNum(num2, (x + clockNumWidhts[num2] + spacing), y);
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
