#include <Arduino.h>
#include "gameInstance.h"

GameInstance game;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
   // Serial.print("ELAPSED TIME: ");
   // Serial.print(millis() / 1000);
    //Serial.print(", ");
    //Serial.println(millis() % 1000);

    game.update();
}
