#include <Arduino.h>
#include "Actor.h"
#include "screenRenderer.h"
#include "controlInterface.h"
#include "entityArray.h"

ScreenRenderer scrRend;
ControlInterface cI;
Player plyr(MAIN, UP, 4, 5, MAIN_TOPX, MAIN_TOPY, MAIN_LEFTX, MAIN_RIGHTX, MAIN_BOTTOMY, PLAYER_HEALTH);
Enemy enmy(plyr, ENEMY2, DOWN, 2, 1, ENEMY2_TOPX, ENEMY2_TOPY, ENEMY2_LEFTX, ENEMY2_RIGHTX, ENEMY2_BOTTOMY, ENEMY_HEALTH);

EntityArray meteorsArr;

void onButtonPress() {
    static unsigned long last_call_time = millis();

    if (millis() - last_call_time >= DELTA_TIME) {
        last_call_time = millis();
        plyr.generateBullet();
    }
}

void setup()
{
    Serial.begin(9600);

    cI.setJoystickClickListener(onButtonPress);

    //enmy.setSpeed(2);
}

void loop()
{
   // Serial.print("ELAPSED TIME: ");
   // Serial.print(millis() / 1000);
    //Serial.print(", ");
    //Serial.println(millis() % 1000);

    static bool bled = false;
    if (!bled) {
        plyr.bleed();
        bled = true;
    }

    static unsigned long st_time = 0;
    /*
    if (millis() - st_time > 1000) {
        st_time = millis();

        Entity *eRef = new Entity(METEOR, DOWN, 3, 0, METEOR_TOPX, METEOR_TOPY);

        meteorsArr.pushBack(eRef);
    }

    for (int i = meteorsArr.getSize() - 1; i >= 0 ; i--) {
            meteorsArr[i].drawShape(scrRend);
    }
    */
    static unsigned long bullet_time = 0;
    if (millis() - bullet_time >= 2 * FOLLOW_DELTA_TIME) {
        bullet_time = millis();
        enmy.generateBullet();
    }

    enmy.draw(scrRend);
    plyr.draw(scrRend);

    scrRend.updateScreen();
    delay(100);

    enmy.update(scrRend);

    /*
    for (int i = meteorsArr.getSize() - 1; i >= 0 ; i--) {
        if (plyr.isColliding(meteorsArr[i])) {
            plyr.bleed();
            meteorsArr[i].clearShape(scrRend);

            meteorsArr.remove(i);
            continue;
        }

        if (meteorsArr[i].updatePosition(scrRend) == OUT_OF_BOUNDS)
            meteorsArr.remove(i);
    }
    */

    cI.updateState();

    int deltaX = cI.getDeltaX(),
        deltaY = cI.getDeltaY();

    bool noMove = true;

    //if we had left movement we cannot have right movement at the same time
    if (deltaX == JOYSTICK_LEFT) {
        plyr.update(scrRend, MOVED_LEFT, &enmy, NULL);
        //plyr.update(scrRend, MOVED_LEFT, NULL, &meteorsArr);
        noMove = false;
    }
    else if (deltaX == JOYSTICK_RIGHT) {
        plyr.update(scrRend, MOVED_RIGHT, &enmy, NULL);
       // plyr.update(scrRend, MOVED_RIGHT, NULL, &meteorsArr);
        noMove = false;
    }

    //if we had down movement we cannot have up movement at the same time
    if (deltaY == JOYSTICK_DOWN) {
        plyr.update(scrRend, MOVED_DOWN, &enmy, NULL);
        //plyr.update(scrRend, MOVED_DOWN, NULL, &meteorsArr);
        noMove = false;
    }
    else if (deltaY == JOYSTICK_UP) {
        plyr.update(scrRend, MOVED_UP, &enmy, NULL);
        //plyr.update(scrRend, MOVED_UP, NULL, &meteorsArr);
        noMove = false;
    }

    if (noMove) {
        plyr.update(scrRend, NO_MOVEMENT, &enmy, NULL);
        //plyr.update(scrRend, NO_MOVEMENT, NULL, &meteorsArr);
    }
}
