
#ifndef OLED_H
#define OLED_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

extern Adafruit_SH1106G display;

// screen segments start coordinates
extern const int segmentsCoords[7][2]; // segments 5 & 6 (index 4 & 5) are not used in usual overlay, only for specific ones

// screen settings
void setDisplayContrast(uint8_t value);

// universal big clock numbers functions
void displayClockNum(int num, int x=0, int y=0);
void displayClockNums(int nums, int x=0, int y=0, int spacing=8);
void displayMiddleDot(bool doubleDot = false);

// universal text display functions
void displayText(String text, int x=0, int y=0, int fontsize=1);

// icon specific funcitions
void displayBatIcon(int level, int x=0, int y=0); // level 0 displays charging icon, the rest is 1-5 bars

#endif
