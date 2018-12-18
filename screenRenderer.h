#ifndef SCREENRENDERER_H_INCLUDED
#define SCREENRENDERER_H_INCLUDED

#include "LedControl.h"
#include "LiquidCrystal.h"

// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

#define NO_DRIVERS 1
#define DIN_PIN 12
#define CLK_PIN 11
#define LOAD_PIN 10

#define LCD_RS 4
#define LCD_ENABLE 5
#define LCD_DB4 6
#define LCD_DB5 7
#define LCD_DB6 13
#define LCD_DB7 8

#define V0_PIN 9

#define LCD_NO_COLS 16
#define LCD_NO_ROWS 2
#define LCD_CONTRAST 90

#define HEART_SYMBOL 1

#define LIVES_ROW 0
#define LEVEL_SCORE_ROW 1
#define LEVEL_COL 0
#define SCORE_COL 15

class ScreenRenderer {
    const int LINES, COLS, DRIVER_ADDR;
    byte rows[8];
    LedControl lc;
    LiquidCrystal LCD;

public :
    ScreenRenderer();
    void updateScreen();
    void setPixel(int L, int C);
    void clearPixel(int L, int C);
    void togglePixel(int L, int C);
    int getLines();
    int getCols();

    void LCDUpdate(int life, byte level, unsigned long score);
    void LCDForcedUpdate(int life, byte level, unsigned long score);

    void LCDPrintLives(int);
    void LCDPrintLevel(byte);
    void LCDPrintScore(unsigned long);
    void LCDPrintMessage(const char*);
    void LCDPrintMultiLineMessage(const char*, const char*);
};

#endif // SCREENRENDERER_H_INCLUDED
