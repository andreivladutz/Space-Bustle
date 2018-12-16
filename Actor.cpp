#include "Actor.h"
#include "screenRenderer.h"

/********************
 *   ENTITY METHODS  *
 ********************/

Entity :: Entity(int dir, int pX, int pY)
        : posX(pX), posY(pY), direction(dir), speed(CONSTANT_SPEED) {}

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
    int offsetY = MIDDLE_Y - topY,
        offsetX = MIDDLE_X - topX;

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

void Entity :: clearShape(ScreenRenderer &sR) {
    toggleShape(sR, false);
}

void Entity :: setSpeed(int spd) {
    speed = spd;
}

void Entity :: setPosX(int newPosX) { posX = newPosX; }
void Entity :: setPosY(int newPosY) { posY = newPosY; }

//don't care about the MOVEMENT parameter, we only move down or up
bool Entity :: updatePosition(ScreenRenderer &sR, MOVEMENT) {
    //firstly, we clear our old position
    toggleShape(sR, false);
    posY -= direction * speed;

    if (posY < 0 || posY > MAX_Y)
        return OUT_OF_BOUNDS;

    return IN_BOUNDS;
}

//we check if any point of left entity and right entity overlap
bool Entity :: isColliding(Entity &E) {
    int leftOffsetY = MIDDLE_Y - topY,
        leftOffsetX = MIDDLE_X - topX,
        rightOffsetY = MIDDLE_Y - E.topY,
        rightOffsetX = MIDDLE_X - E.topX;

    for (int leftLineOffs = -1; leftLineOffs <= 1; leftLineOffs++)
        for (int leftColOffs = -1; leftColOffs <= 1; leftColOffs++)
            for (int rightLineOffs = -1; rightLineOffs <= 1; rightLineOffs++)
                for (int rightColOffs = -1; rightColOffs <= 1; rightColOffs++)
                    if (shape[MIDDLE_Y + leftLineOffs][MIDDLE_X + leftColOffs] && E.shape[MIDDLE_Y + rightLineOffs][MIDDLE_X + rightColOffs]) {
                        if (posY + leftLineOffs + leftOffsetY == E.posY + rightLineOffs + rightOffsetY &&
                            posX + leftColOffs + leftOffsetX == E.posX + rightColOffs + rightOffsetX)
                            return true;
                    }
    return false;
}

/********************
 *   BULLET METHODS  *
 ********************/

Bullet :: Bullet(int dir, int pX, int pY)
        : Entity(dir, pX, pY) {}

//check if any point of the right E shape overlaps the bullet point
bool Bullet :: isColliding(Entity &E) {
    int offsetY = MIDDLE_Y - E.topY,
        offsetX = MIDDLE_X - E.topX;

    for (int lineOffs = -1; lineOffs <= 1; lineOffs++)
        for (int colOffs = -1; colOffs <= 1; colOffs++)
            if (E.shape[MIDDLE_Y + lineOffs][MIDDLE_X + colOffs])
                if (posY == E.posY + lineOffs + offsetY && posX == E.posX + colOffs + offsetX)
                    return true;
    return false;
}

void Bullet :: toggleShape(ScreenRenderer &sR, bool setExpr) {
    if (setExpr)
        sR.setPixel(posY, posX);
    else
        sR.clearPixel(posY, posX);
}

/********************
 *   ACTOR METHODS  *
 ********************/

Actor :: Actor(const int init_shape[3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life)
        : Entity(init_shape, dir, pX, pY, tX, tY), life(life), bottomY(bY), leftX(lX), rightX(rX), lastShown(true),
        startedBleeding(false), bullet_time(0), blink_time(0), bleed_time(0){}

//the function keeps track of its last display value (show or hide)
//and changes it once the desired delay time (of ONE_BLEED) has passed
void Actor :: blinkShape(ScreenRenderer &sR) {
    if (blink_time - millis() > ONE_BLEED) {
        blink_time = millis();

        toggleShape(sR, lastShown);
        lastShown = !lastShown;
    }
}

bool Actor :: isBleeding() {
    if (bleeding) {
        //first time we observe bleeding
        if (!startedBleeding) {
            startedBleeding = true;
            bleed_time = millis();
        }
        //already started. check if we should "stop bleeding"
        else {
            if (millis() - bleed_time >= BLEED_TIME) {
                startedBleeding = false;
                bleeding = false;
            }
        }
        return true;
    }

    return false;
}

//the actor has a temporary invincibility while bleeding
void Actor :: bleed() {
    if (bleeding)
        return;

    life--;
    bleeding = true;
}

int Actor :: getLife() {
    return life;
}

void Actor :: drawShape(ScreenRenderer &sR) {
    if (isBleeding())
        blinkShape(sR);
    else
        Entity :: drawShape(sR);
}

//generate a bullet at the top of the actor going
//same direction as the actor UP or DOWN
void Actor :: generateBullet() {
    if (millis() - bullet_time >= BULLET_GENERATION_INTERVAL) {
        bullet_time = millis();

        Entity *eRef = new Bullet(direction, posX, posY - direction + 1);

        bulletsArr.pushBack(eRef);
    }
}

void Actor :: drawBullets(ScreenRenderer &sR) {
    for (int i = bulletsArr.getSize() - 1; i >= 0 ; i--) {
            bulletsArr[i].drawShape(sR);
    }
}

void Actor :: updateBullets(ScreenRenderer &sR) {
    for (int i = bulletsArr.getSize() - 1; i >= 0 ; i--) {
        if (bulletsArr[i].updatePosition(sR) == OUT_OF_BOUNDS)
            bulletsArr.remove(i);
    }
}

bool Actor :: updatePosition(ScreenRenderer &sR, MOVEMENT whichWay) {
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

    if (whichWay == MOVED_LEFT && (leftOffset + posX - (LEFT * speed)) >= 0)
        posX -= (LEFT * speed);
    if (whichWay == MOVED_RIGHT && (rightOffset + posX - (RIGHT * speed)) < MAX_X)
        posX -= (RIGHT * speed);
    if (whichWay == MOVED_UP && (posY - (UP * speed)) >= 0)
        posY -= (UP * speed);
    if (whichWay == MOVED_DOWN && (bottomOffset + posY - (DOWN * speed)) < MAX_Y)
        posY -= (DOWN * speed);

    //always in bounds
    return IN_BOUNDS;
}

void Actor :: draw(ScreenRenderer &sR) {
    drawBullets(sR);
    drawShape(sR);
}

/********************
 *   PLAYER METHODS  *
 ********************/

Player :: Player(const int init_shape[3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life)
        : Actor(init_shape, dir, pX, pY, tX, tY, lX, rX, bY, life) {}

/*
    if there is no enemy spawned currently or we're not in meteor-spawning mode NULL should pe passed as parameter
*/
void Player :: updatePosition(ScreenRenderer &sR, MOVEMENT whichWay, Enemy *currEnemy, EntityArray *meteors) {
    Actor :: updatePosition(sR, whichWay);

    //if player collides with enemy they both bleed
    if (currEnemy) {
        if (isColliding(*currEnemy)) {
            currEnemy->bleed();
            bleed();
        }
    }
    //if player collides with any meteor it bleeds and the meteor gets destroyed
    else if (meteors) {
        for (int i = (*meteors).getSize() - 1; i >= 0; i--)
            if (isColliding((*meteors)[i])) {
                bleed();

                (*meteors)[i].clearShape(sR);
                (*meteors).remove(i);
            }
    }
}

/*
    if there is no enemy spawned currently or we're not in meteor-spawning mode NULL should pe passed as parameter
*/
void Player :: updateBullets(ScreenRenderer &sR, Enemy *currEnemy, EntityArray *meteors) {
    Actor :: updateBullets(sR);

    //if any bullet collides with the enemy, the enemy bleeds and the bullet gets destroyed
    if (currEnemy) {
        for (int i = bulletsArr.getSize() - 1; i >= 0 ; i--)
            if (bulletsArr[i].isColliding(*currEnemy)) {
                currEnemy->bleed();

                bulletsArr[i].clearShape(sR);
                bulletsArr.remove(i);
            }
    }
    //if any bullet collides with any meteor they both get destroyed
    else if (meteors) {
        for (int i = bulletsArr.getSize() - 1; i >= 0 ; i--) {
            bool gotDestroyed = false;

            for (int j = (*meteors).getSize() - 1; j >= 0; j--)
                if (bulletsArr[i].isColliding((*meteors)[j])) {
                    bulletsArr[i].clearShape(sR);
                    bulletsArr.remove(i);

                    (*meteors)[j].clearShape(sR);
                    (*meteors).remove(j);

                    gotDestroyed = true;

                    break;
                }
            if (gotDestroyed)
                continue;
        }
    }
}

/*
    if there is no enemy spawned currently or we're not in meteor-spawning mode NULL should pe passed as parameter
*/
void Player :: update(ScreenRenderer &sR, MOVEMENT whichWay, Enemy *currEnemy, EntityArray *meteors) {
    //if the player didn't move we don't have to update its position
    if (whichWay != NO_MOVEMENT)
        updatePosition(sR, whichWay, currEnemy, meteors);
    //we update the bullets no matter what
    updateBullets(sR, currEnemy, meteors);

}

/********************
 *   ENEMY METHODS  *
 ********************/

Enemy :: Enemy(Player &mainPlyr, const int init_shape[3][3], int dir, int pX, int pY, int tX, int tY, int lX, int rX, int bY, int life)
        : Actor(init_shape, dir, pX, pY, tX, tY, lX, rX, bY, life), mainPlayer(mainPlyr) {}

/*
    deltaToPlayerX negative => main player to the right
    deltaToPlayerX positive => main player to the left
*/
bool Enemy :: updatePosition(ScreenRenderer &sR, MOVEMENT whichWay) {
    static unsigned long start_time = 0;

    if (millis() - start_time >= FOLLOW_DELTA_TIME) {
        start_time = millis();

        int deltaToPlayerX = posX - mainPlayer.posX;

        if (deltaToPlayerX < 0)
            Actor :: updatePosition(sR, MOVED_RIGHT);
        else if (deltaToPlayerX > 0)
            Actor :: updatePosition(sR, MOVED_LEFT);

        if (isColliding(mainPlayer)) {
            mainPlayer.bleed();
            bleed();
        }
    }

    return IN_BOUNDS;
}

void Enemy :: updateBullets(ScreenRenderer &sR) {
    Actor :: updateBullets(sR);

    //if any bullet collides with the player, the player bleeds and the bullet gets destroyed
    for (int i = bulletsArr.getSize() - 1; i >= 0 ; i--)
        if (bulletsArr[i].isColliding(mainPlayer)) {
            mainPlayer.bleed();

            bulletsArr[i].clearShape(sR);
            bulletsArr.remove(i);
        }
}

void Enemy :: update(ScreenRenderer &sR) {
    updatePosition(sR);
    updateBullets(sR);
}
