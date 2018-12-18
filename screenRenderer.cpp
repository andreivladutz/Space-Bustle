#include "screenRenderer.h"

//small function to show a reversed number on the LCD
int reverseInt(int no) {
    int reversed = 0;

    while (no) {
        reversed = reversed * 10 + no % 10;
        no /= 10;
    }

    return reversed;
}

uint8_t heart[8] = {
    B11011,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
};

/*
    init constants
    set the matrix on 0
    initialize the LedControl
*/
ScreenRenderer :: ScreenRenderer() : LINES(8), COLS(8), DRIVER_ADDR(0),
        lc(DIN_PIN, CLK_PIN, LOAD_PIN, NO_DRIVERS),
        LCD(LCD_RS, LCD_ENABLE, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7)
{
    for (int i = 0; i < LINES; i++)
        rows[i] = B00000000;
    lc.shutdown(DRIVER_ADDR, false); // turn off power saving, enables display
    lc.setIntensity(DRIVER_ADDR, 8); // sets brightness (0~15 possible values)

    LCD.createChar(HEART_SYMBOL, heart);
    LCD.begin(LCD_NO_COLS, LCD_NO_ROWS);
    LCD.clear();

    pinMode(V0_PIN, OUTPUT);
    analogWrite(V0_PIN, LCD_CONTRAST);
}

/*********************************
 *   METHODS FOR THE LED MATRIX  *
 ********************************/

void ScreenRenderer :: updateScreen() {
    for (int i = 0; i < LINES; i++)
        lc.setRow(DRIVER_ADDR, i, rows[i]);
}

/*
    if the pixel the user is trying to change
    is out of the game boundary we simply ignore it
*/

//set bit on col position
void ScreenRenderer :: setPixel(int line, int col) {
    if (line < 0 || line >= LINES ||
        col < 0 || col >= COLS)
            return;

    rows[line] |= 1 << col;
}

//clear bit on col position
void ScreenRenderer :: clearPixel(int line, int col) {
    if (line < 0 || line >= LINES ||
    col < 0 || col >= COLS)
        return;

    byte clr = (1 << col);
    rows[line] &= ~clr;
}

//toggle bit on col position
void ScreenRenderer :: togglePixel(int line, int col) {
    if (line < 0 || line >= LINES ||
    col < 0 || col >= COLS)
        return;

    rows[line] ^= 1 << col;
}

int ScreenRenderer :: getLines() {
    return LINES;
}

int ScreenRenderer :: getCols() {
    return COLS;
}

/**************************
 *   METHODS FOR THE LCD  *
 *************************/

void ScreenRenderer :: LCDUpdate(int life, byte level, int score) {
    static int last_life = 0;
    static byte last_level = 0;
    static int last_score = 1;

    if (life != last_life) {
        last_life = life;
        LCDPrintLives(life);
    }
    if (level != last_level) {
        last_level = level;
        LCDPrintLevel(level);
    }
    if (score != last_score) {
        last_score = score;
        LCDPrintScore(score);
    }
}

void ScreenRenderer :: LCDForcedUpdate(int life, byte level, int score) {
    LCD.clear();
    LCDPrintLives(life);
    LCDPrintLevel(level);
    LCDPrintScore(score);
}

void ScreenRenderer :: LCDPrintLives(int life) {
    LCD.setCursor(0, LIVES_ROW);

    //clear row
    for (int i = 0; i < LCD_NO_COLS; i++)
        LCD.write(' ');

    //center Lives display
    LCD.setCursor((LCD_NO_COLS - life) / 2, LIVES_ROW);

    for (int i = 0; i < life; i++)
        LCD.write(HEART_SYMBOL);
}

void ScreenRenderer :: LCDPrintLevel(byte level) {
    LCD.setCursor(LEVEL_COL, LEVEL_SCORE_ROW);
    LCD.print("LVL:");
    LCD.print(level);
}

void ScreenRenderer :: LCDPrintScore(int score) {
    LCD.rightToLeft();
    LCD.setCursor(SCORE_COL, LEVEL_SCORE_ROW);
    LCD.print("         ");
    LCD.setCursor(SCORE_COL, LEVEL_SCORE_ROW);

    int alt_score = score;

    //if score has some ending zeros we print those before
    //printing the reversed number (which won't have leading zeros)
    if (score != 0)
        while (alt_score % 10 == 0) {
            LCD.write('0');
            alt_score /= 10;
        }

    //REVERSING ALL OUTPUT SO IT PRINTS WELL
    LCD.print(reverseInt(score));
    LCD.print(":RCS");

    LCD.leftToRight();
}

void ScreenRenderer :: LCDPrintMessage(const char *txt) {
    LCD.clear();
    LCD.print(txt);
}

void ScreenRenderer :: LCDPrintMultiLineMessage(const char *line1, const char *line2) {
    LCD.clear();
    LCD.print(line1);

    LCD.setCursor(0, 1);
    LCD.print(line2);
}
