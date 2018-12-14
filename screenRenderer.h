#ifndef SCREENRENDERER_H_INCLUDED
#define SCREENRENDERER_H_INCLUDED

#include "LedControl.h"

// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

#define NO_DRIVERS 1
#define DIN_PIN 12
#define CLK_PIN 11
#define LOAD_PIN 10

class ScreenRenderer {
    const int LINES, COLS, DRIVER_ADDR;
    byte rows[8];
    LedControl lc;


public :
    ScreenRenderer();
    void updateScreen();
    void setPixel(int L, int C);
    void clearPixel(int L, int C);
    void togglePixel(int L, int C);
    int getLines();
    int getCols();
};

#endif // SCREENRENDERER_H_INCLUDED
