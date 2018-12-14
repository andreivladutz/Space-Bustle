#include <Arduino.h>
#include "Actor.h"
#include "screenRenderer.h"
#include "controlInterface.h"
#include "entityArray.h"

ScreenRenderer scrRend;
ControlInterface *cI;
Player plyr(MAIN, UP, 4, 6, MAIN_TOPX, MAIN_TOPY, MAIN_LEFTX, MAIN_RIGHTX, MAIN_BOTTOMY, PLAYER_HEALTH);
volatile int changeState = 0;
volatile int buttonPresses = 0;

EntityList L;

void onButtonPress() {
    //Serial.println("BUTTON PRESSED");
    changeState = !changeState;
    buttonPresses++;
}

void onOutOfBounds(const Entity *self) {
    L.removeCurrent();
    //delete self;
}

void setup()
{
    Serial.begin(9600);
    cI = new ControlInterface();
    cI->setClickListener(onButtonPress);
}

void loop()
{
    static int bled = false;

    if (!bled) {
        bled = true;
        plyr.bleed();
    }

    static unsigned long long st_time = 0;
    static bool last_updated = true;


    if (millis() - st_time > 300 && L.getSize() < 4) {
        st_time = millis();

        Entity *eRef = new Entity(ENEMY1, DOWN, 3, 3, ENEMY1_TOPX, ENEMY1_TOPY);

        eRef->setOutOfBoundsListener(onOutOfBounds);

        L.pushBack(*eRef);
    }

    Serial.println(L.getSize());

    //we check if we have at least one element
    if (!L.currSetted() && L.getSize()){
        L.setOnFirst();
    }

    //make sure the list didn't become empty
    while (L.getSize() && !L.onLast()) {
        if (last_updated) {
            L.peekCurrent().drawShape(scrRend);
            last_updated = false;
        }
        else {
            L.peekCurrent().updatePosition(scrRend);
            L.setNext();

            last_updated = true;
        }
    }

    //special case for the last element
    if (L.getSize() && L.onLast()) {
        if (last_updated) {
            L.peekCurrent().drawShape(scrRend);
            last_updated = false;
        }
        else {
            L.peekCurrent().updatePosition(scrRend);
            L.setOnFirst();

            last_updated = true;
        }
    }


    if (plyr.isBleeding())
        plyr.blinkShape(scrRend);
    else
        plyr.drawShape(scrRend);

    scrRend.updateScreen();
    delay(100);
    //plyr.updatePosition(scrRend);
    //scrRend.updateScreen();

    cI->updateState();
    //Serial.println();
    //Serial.println(cI->getDeltaY());

    int deltaX = cI->getDeltaX(),
        deltaY = cI->getDeltaY();

    if (deltaX == JOYSTICK_LEFT)
        plyr.updatePosition(scrRend, MOVED_LEFT);
    else if (deltaX == JOYSTICK_RIGHT)
        plyr.updatePosition(scrRend, MOVED_RIGHT);


    if (deltaY == JOYSTICK_DOWN)
        plyr.updatePosition(scrRend, MOVED_DOWN);
    else if (deltaY == JOYSTICK_UP)
        plyr.updatePosition(scrRend, MOVED_UP);


   // if(changeState) {
        //Serial.println("Button pressed\n");
   // }
   //Serial.println("buttonPresses");
   //Serial.println(buttonPresses);

    //delay(100);
}
