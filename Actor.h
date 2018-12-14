#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include "screenRenderer.h"

class ScreenRenderer;

/*
    MAIN CHARA

        1
      1 1 1
      1   1
*/
const int MAIN[3][3] = {
    { 0, 1, 0 },
    { 1, 1, 1 },
    { 1, 0, 1 }
}, MAIN_TOPX = 1, MAIN_TOPY = 0,
    MAIN_LEFTX = 0, MAIN_RIGHTX = 2,
    MAIN_BOTTOMY = 2;
/*
    ENEMY 1

      1 1 1
        1
*/
const int ENEMY1[3][3] = {
    { 1, 1, 1 },
    { 0, 1, 0},
    { 0, 0, 0 }
}, ENEMY1_TOPX = 1, ENEMY1_TOPY = 1,
    ENEMY1_LEFTX = 0, ENEMY1_RIGHTX = 2,
    ENEMY1_BOTTOMY = 0;
/*
    ENEMY 2
        1
      1 1 1
*/
const int ENEMY2[3][3] = {
    { 0, 1, 0},
    { 1, 1, 1},
    { 0, 0, 0 }
}, ENEMY2_TOPX = 1, ENEMY2_TOPY = 1,
    ENEMY2_LEFTX = 0, ENEMY2_RIGHTX = 2,
    ENEMY2_BOTTOMY = 0;

//the way one entity is taking
#define DOWN -1
#define UP 1
#define LEFT 1
#define RIGHT -1

#define CONSTANT_SPEED 1
#define SHAPE_SIZE 3

#define MAX_X 8
#define MAX_Y 8

//the middle of a shape array
#define MIDDLE_X 1
#define MIDDLE_Y 1

enum MOVEMENT {AUTO_MOVEMENT, MOVED_LEFT, MOVED_RIGHT, MOVED_UP, MOVED_DOWN};

class Entity {
protected :
    //every enity has a shape of max width and height 3
    int shape[3][3];
    //posX and posY are the current positions in the game matrix
    //topX and topY represent the "head of the shape"
    int posX, posY, topX, topY;
    //DOWN OR UP
    int direction;
    int speed;

    //listener to call when entity goes out of the game board
    //like deleting the entity to free memory -> "this" will be passed
    void (*outOfBoundsListener)(const Entity*);
    //if setExpr is true we show the shape
    //else we clear it
    void toggleShape(ScreenRenderer &, bool setExpr);
public :
    //defining the shape and the direction, the current position
    //on the game board and the coords of the "head"
    Entity(const int [3][3], int dir, int pX, int pY, int tX, int tY);

    //drawing the shape on the game board
    void drawShape(ScreenRenderer &);

    //for Entities we have automatic movement
    virtual void updatePosition(ScreenRenderer &, MOVEMENT whichWay = AUTO_MOVEMENT);

    void setPosX(int);
    void setPosY(int);

    void setSpeed(int);

    bool isColliding(Entity&);

    //set the out of bounds listener
    void setOutOfBoundsListener(void (*)(const Entity*));
};

#define BLEED_TIME 1000
#define ONE_BLEED 200

class Actor : public Entity {
protected :
    int life;
    //the coords of the leftmost rightmost and bottom points of the shape
    //to check collision with the border
    int bottomY, leftX, rightX;
    bool bleeding;
public :
    Actor(const int [3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life);
    /*
        caused by collision -> damage is taken so we "bleed"
        life is decremented and we set set bleeding flag which will cause
        a blinking animation (blinkShape should be called instead of drawShape)

        also to check if we "bleed" we should use ifBleeding because it has the logic
        needed to turn of the bleeding flag after some time (we shouldn't bleed forever)
    */
    void bleed();
    //return if bleeding and also keeping a timer to turn off bleed value
    //after BLEED_TIME has passed
    bool isBleeding();

    //toggling the shape on and off. useful for animating bleeding on Actors
    void blinkShape(ScreenRenderer &);

    int getLife();
};

#define PLAYER_HEALTH 10

class Player : public Actor {

public :
    Player(const int [3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life);

    void updatePosition(ScreenRenderer &, MOVEMENT whichWay);
} ;

#endif // ACTOR_H_INCLUDED
