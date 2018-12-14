#include "screenRenderer.h"

/*
    init constants
    set the matrix on 0
    initialize the LedControl
*/
ScreenRenderer :: ScreenRenderer() : LINES(8), COLS(8), DRIVER_ADDR(0),
        lc(DIN_PIN, CLK_PIN, LOAD_PIN, NO_DRIVERS) {
    for (int i = 0; i < LINES; i++)
        rows[i] = B00000000;
    lc.shutdown(DRIVER_ADDR, false); // turn off power saving, enables display
    lc.setIntensity(DRIVER_ADDR, 8); // sets brightness (0~15 possible values)
}

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
