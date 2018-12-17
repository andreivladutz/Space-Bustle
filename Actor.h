#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED

#include "entityArray.h"
#include <Arduino.h>

//class EntityArray;

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

#define NO_OF_ENEMIES 2

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

const int METEOR[3][3] = {
    { 1, 1, 0 },
    { 1, 1, 0 },
    { 0, 0, 0 }
}, METEOR_TOPX = 1, METEOR_TOPY = 1;

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

#define OUT_OF_BOUNDS true
#define IN_BOUNDS !OUT_OF_BOUNDS

enum MOVEMENT {NO_MOVEMENT, AUTO_MOVEMENT, MOVED_LEFT, MOVED_RIGHT, MOVED_UP, MOVED_DOWN};

class Bullet;
class Enemy;

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

    //if setExpr is true we show the shape
    //else we clear it
    virtual void toggleShape(ScreenRenderer &, bool setExpr);
    //protected constructor for the Bullet class to call
    //the shape array becomes useless so we don't initialize it
    Entity(int dir, int pX, int pY);
public :
    //defining the shape and the direction, the current position
    //on the game board and the coords of the "head"
    Entity(const int [3][3], int dir, int pX, int pY, int tX, int tY);

    //drawing the shape on the game board
    void drawShape(ScreenRenderer &);
    void clearShape(ScreenRenderer &);

    //for Entities we have automatic movement
    virtual bool updatePosition(ScreenRenderer &, MOVEMENT whichWay = AUTO_MOVEMENT);

    void setPosX(int);
    void setPosY(int);

    void setSpeed(int);

    virtual bool isColliding(Entity&);

    virtual ~Entity() {}

    friend class Bullet;
    friend class Enemy;
};

/*
    if we check collision with a bullet
    the bullet has to be on the left-hand
    side of the isColliding call
*/
class Bullet : public Entity {
protected :
    void toggleShape(ScreenRenderer &, bool setExpr);
public :
    Bullet(int dir, int pX, int pY);
    bool isColliding(Entity&);
};

#define BLEED_TIME 1000
#define ONE_BLEED 200

#define BULLET_GENERATION_INTERVAL 300

class Actor : public Entity {
protected :
    int life;
    //the coords of the leftmost rightmost and bottom points of the shape
    //to check collision with the border
    int bottomY, leftX, rightX;
    bool bleeding;

    EntityArray bulletsArr;

    //flags for the bleeding effect
    bool lastShown, startedBleeding;

    //FOR TIMERS
    unsigned long bullet_time,
        blink_time, bleed_time;
public :
    Actor(const int [3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life);
    ~Actor();
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
    void drawShape(ScreenRenderer &sR);

    bool updatePosition(ScreenRenderer &, MOVEMENT whichWay);

    int getLife();

    void generateBullet();
    void drawBullets(ScreenRenderer &sR);
    void clearBullets(ScreenRenderer &sR);
    void updateBullets(ScreenRenderer &sR);

    void draw(ScreenRenderer &);
};

class Enemy;

#define PLAYER_HEALTH 10

class Player : public Actor {

public :
    Player(const int [3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life);

    /*
        the update function updates both the bullets
        and the position of the player
    */
    void update(ScreenRenderer &, MOVEMENT whichWay, Enemy *currEnemy, EntityArray *meteors);

    /*
        the player updatePosition verifies collision with the current enemy if there is one, else
                checks collision with spawned meteors and calls Actor :: updatePosition
    */
    void updatePosition(ScreenRenderer &, MOVEMENT whichWay, Enemy *currEnemy, EntityArray *meteors);

    /*
        checks collision with currEnemy or walks through the meteors array checking collision
    */
    void updateBullets(ScreenRenderer &, Enemy *currEnemy, EntityArray *meteors);
};

#define ENEMY_HEALTH 3

#define FOLLOW_DELTA_TIME 1000

class Enemy : public Actor {
    Player &mainPlayer;
public :
    Enemy(Player&, const int [3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life);

    bool updatePosition(ScreenRenderer &, MOVEMENT whichWay = AUTO_MOVEMENT);
    void updateBullets(ScreenRenderer &sR);
    void update(ScreenRenderer &sR);
};

#endif // ACTOR_H_INCLUDED
