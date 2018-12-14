#include "Actor.h"

Entity :: Entity(const int shape_init[3][3], int dir, int pX, int pY, int tX, int tY)
        : posX(pX), posY(pY), topX(tX), topY(tY), direction(dir), speed(CONSTANT_SPEED) {

    for (int i = 0; i < SHAPE_SIZE; i++)
        for (int j = 0; j < SHAPE_SIZE; j++)
            shape[i][j] = shape_init[i][j];
}

/*
    drawing the shape with topX at posX and topY at posY

    looking at all 8 pixels around MID_X and MID_Y in the 3 by 3 shape matrix
    and setting pixels on the game board, offsetting accordingly
*/
void Entity :: toggleShape(ScreenRenderer &sR, bool setExpr) {
    int offsetX = topY - MIDDLE_Y,
        offsetY = topX - MIDDLE_X;

    for (int lineOffs = -1; lineOffs <= 1; lineOffs++)
        for (int colOffs = -1; colOffs <= 1; colOffs++)
            if (shape[MIDDLE_Y + lineOffs][MIDDLE_X + colOffs]) {
                if (setExpr)
                    sR.setPixel(posY + lineOffs + offsetY, posX + colOffs + offsetX);
                else
                    sR.clearPixel(posY + lineOffs + offsetY, posX + colOffs + offsetX);
            }
}

void Entity :: drawShape(ScreenRenderer &sR) {
    toggleShape(sR, true);
}

void Entity :: setSpeed(int spd) {
    speed = spd;
}

void Entity :: setPosX(int newPosX) { posX = newPosX; }
void Entity :: setPosY(int newPosY) { posY = newPosY; }

//don't care about the MOVEMENT parameter, we only move down or up
void Entity :: updatePosition(ScreenRenderer &sR, MOVEMENT) {
    //firstly, we clear our old position
    toggleShape(sR, false);
    posY -= direction * speed;

    if (posY < 0 || posY > MAX_Y)
        return outOfBoundsListener(this);
}

void Entity :: setOutOfBoundsListener(void (*func)(const Entity*)) {
    outOfBoundsListener = func;
}


Actor :: Actor(const int init_shape[3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life)
        : Entity(init_shape, dir, pX, pY, tX, tY), leftX(lX), rightX(rX), bottomY(bY), life(life) {}

//the function keeps track of its last display value (show or hide)
//and changes it once the desired delay time (of ONE_BLEED) has passed
void Actor :: blinkShape(ScreenRenderer &sR) {
    static bool show = true;
    static bool started = false;
    static unsigned long long start_time;

    if (!started) {
        started = true;
        start_time = millis();

        toggleShape(sR, show);
        show = !show;
    }
    else {
        if (start_time - millis() > ONE_BLEED)
            started = false;
    }
}

bool Actor :: isBleeding() {
    static bool started = false;
    static unsigned long long start_time;

    if (bleeding) {
        //first time we observe bleeding
        if (!started) {
            started = true;
            start_time = millis();
        }
        //already started. check if we should "stop bleeding"
        else {
            if (millis() - start_time > BLEED_TIME) {
                started = false;
                bleeding = false;
            }
        }
        return true;
    }

    return false;
}

void Actor :: bleed() {
    life--;
    bleeding = true;
}

int Actor :: getLife() {
    return life;
}


Player :: Player(const int init_shape[3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life)
        : Actor(init_shape, dir, pX, pY, tX, tY, lX, rX, bY, life) {}

void Player :: updatePosition(ScreenRenderer &sR, MOVEMENT whichWay) {
    //firstly, we clear our old position
    toggleShape(sR, false);

    /*
        Keeping count of the offset in the shape matrix
        what is the relative position of leftX to topX
        which is positioned at posX, etc.
    */
    int leftOffset = leftX - topX,
        rightOffset = rightX - topX,
        bottomOffset = bottomY - topY;

    if (whichWay == MOVED_LEFT && (leftOffset + posX - (LEFT * speed)) > 0)
        posX -= (LEFT * speed);
    if (whichWay == MOVED_RIGHT && (rightOffset + posX - (RIGHT * speed)) <= MAX_X)
        posX -= (RIGHT * speed);
    if (whichWay == MOVED_UP && (posY - (UP * speed)) > 0)
        posY -= (UP * speed);
    if (whichWay == MOVED_DOWN && (bottomOffset + posY - (DOWN * speed)) <= MAX_Y)
        posY -= (DOWN * speed);

}
