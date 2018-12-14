#ifndef CONTROLINTERFACE_H_INCLUDED
#define CONTROLINTERFACE_H_INCLUDED

#include <Arduino.h>

#define VRx_PIN A0
#define VRy_PIN A1
#define SW_PIN 2
#define SW_INTERRUPT_ID 0

/*
    deltaX negative => right
    deltaX positive => left
    deltaY negative => down
    deltaY positive => up
*/
#define JOYSTICK_RIGHT -1
#define JOYSTICK_LEFT 1
#define JOYSTICK_DOWN -1
#define JOYSTICK_UP 1
#define JOYSTICK_NOMOVE 0

//if delta is less than MOVE_ERROR
//we consider that we have no movement
#define MOVE_ERROR 350

#define DELTA_TIME 100

#define DEFAULT_X 523
#define DEFAULT_Y 507

class ControlInterface {
    int deltaX, deltaY;
public :
    //constructor initializes pins for analog/digital read
    //also reads initial state of stationary joystick
    ControlInterface();
    //sets deltaX and deltaY
    //has to be called with getDeltaX/getDeltaY to get something out
    void updateState();

    //we check for error and return JOYSTICK_*direction* accordingly
    //keeping a timer so we don't return movement every millisecond
    int getDeltaX();
    int getDeltaY();

    //set a function to be called on interrupt caused by button press
    void setClickListener(void (*)());

};

#endif // CONTROLINTERFACE_H_INCLUDED
