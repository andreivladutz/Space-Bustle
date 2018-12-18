#include "controlInterface.h"

ControlInterface :: ControlInterface() {
    pinMode(VRx_PIN, INPUT);
    pinMode(VRy_PIN, INPUT);
    pinMode(SW_PIN, INPUT_PULLUP);

    pinMode(GAME_BUTTON_PIN, INPUT);
}

/*
    deltaX negative => right
    deltaX positive => left
    deltaY negative => down
    deltaY positive => up
*/
void ControlInterface :: updateState() {
    deltaX = analogRead(VRx_PIN) - DEFAULT_X;
    deltaY = analogRead(VRy_PIN) - DEFAULT_Y;
}

int ControlInterface :: getDeltaX() {
    static unsigned long last_return_time = 0;

    if (deltaX >= MOVE_ERROR || deltaX <= -MOVE_ERROR) {
        if (millis() - last_return_time >= DELTA_TIME) {
            last_return_time = millis();

            if (deltaX > 0)
                return JOYSTICK_LEFT;
            else if (deltaX < 0)
                return JOYSTICK_RIGHT;
        }
        return JOYSTICK_NOMOVE;
   }

    return JOYSTICK_NOMOVE;
}
int ControlInterface :: getDeltaY() {
    static unsigned long last_return_time = 0;

    if (deltaY >= MOVE_ERROR || deltaY <= -MOVE_ERROR) {
        if (millis() - last_return_time >= DELTA_TIME) {
            last_return_time = millis();

            if (deltaY > 0)
                return JOYSTICK_UP;
            else if (deltaY < 0)
                return JOYSTICK_DOWN;
        }
        return JOYSTICK_NOMOVE;
    }

    return JOYSTICK_NOMOVE;
}

/*
    if pressed, the joystick button goes in LOW state
*/
void ControlInterface :: setJoystickClickListener(void (*func)()) {
    attachInterrupt(SW_INTERRUPT_ID, func, FALLING);
}

/*
    if pressed, the game button goes in HIGH state
*/
void ControlInterface :: setGameButtonClickListener(void (*func)()) {
    attachInterrupt(GAME_BUTTON_INTERRUPT_ID, func, RISING);
}
