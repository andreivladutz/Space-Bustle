#include "gameInstance.h"

GameInstance *thisInstance;

void onButtonPress() {
    static unsigned long last_call_time = millis();

    if (millis() - last_call_time >= DELTA_TIME) {
        last_call_time = millis();
        thisInstance->playerGenerateBullet();
    }
}

GameInstance :: GameInstance()
        : plyr(MAIN, UP, INITIAL_PLAYERX, INITIAL_PLAYERY, MAIN_TOPX, MAIN_TOPY, MAIN_LEFTX, MAIN_RIGHTX, MAIN_BOTTOMY, PLAYER_HEALTH),
        enmy(NULL), meteorShowerDuration(LEVEL1_METEOR_SHOWER), meteorSpawnTime(LEVEL1_METEOR_SPAWN), bulletSpawnTime(LEVEL1_BULLET_SPAWN),
        showerStartTime(millis()), gameState(METEOR_SHOWER), lastGameState(METEOR_SHOWER), level(1), speed(CONSTANT_SPEED)
{
    randomSeed(analogRead(0));

    thisInstance = this;
    cI.setJoystickClickListener(onButtonPress);
}

/*******************************
 *   GAME LOGIC AND RENDERING  *
 ******************************/

void GameInstance :: update() {
    checkGameState();

    if (gameState == GAME_PAUSED || gameState == GAME_OVER)
        return;

    drawGameObjects();

    scrRend.updateScreen();
    delay(UPDATE_INTERVAL);

    updateGameObjects();
}

void GameInstance :: checkGameState() {
    if (plyr.getLife() <= 0) {
        gameState = GAME_OVER;
        deleteEnemy();
        deleteMeteors();
    }

    //if we defeated the enemy we increase the level and difficulty
    else if (gameState == ENEMY_FIGHT && enmy->getLife() <= 0) {
        deleteEnemy();

        gameState = METEOR_SHOWER;
        increaseLevel();
        showerStartTime = millis();
    }

    else if (gameState == METEOR_SHOWER && millis() - showerStartTime >= meteorShowerDuration) {
        gameState = ENEMY_FIGHT;

        deleteMeteors();
        generateEnemy();
    }
}

void GameInstance :: increaseLevel() {
    level++;

    meteorShowerDuration *= level;

    bulletSpawnTime -= BULLET_SPAWN_DECREASE_TIME * level;
    meteorShowerDuration -= METEOR_SPAWN_DECREASE_TIME * level;

    //we increase speed every two levels
    if (level % SPEED_INCREASE_LEVEL_INTERVAL == 0)
        speed *= (level - 1);
}

/*******************************
 *   METHODS FOR GAME OBJECTS  *
 *******************************/

void GameInstance :: drawGameObjects() {
    if (gameState == METEOR_SHOWER) {
        generateMeteor();
        drawMeteors();
    }

    if (gameState == ENEMY_FIGHT) {
        enemyGenerateBullet();
        enmy->draw(scrRend);
    }

    plyr.draw(scrRend);
}

void GameInstance :: updateGameObjects() {
    if (gameState == ENEMY_FIGHT) {
        enmy->update(scrRend);
    }

    if (gameState == METEOR_SHOWER)
        updateMeteors();

    cI.updateState();

    int deltaX = cI.getDeltaX(),
        deltaY = cI.getDeltaY();

    bool noMove = true;

    //if we had left movement we cannot have right movement at the same time
    if (deltaX == JOYSTICK_LEFT) {
        plyr.update(scrRend, MOVED_LEFT, enmy, &meteorsArr);
        noMove = false;
    }
    else if (deltaX == JOYSTICK_RIGHT) {
        plyr.update(scrRend, MOVED_RIGHT, enmy, &meteorsArr);
        noMove = false;
    }

    //if we had down movement we cannot have up movement at the same time
    if (deltaY == JOYSTICK_DOWN) {
        plyr.update(scrRend, MOVED_DOWN, enmy, &meteorsArr);
        noMove = false;
    }
    else if (deltaY == JOYSTICK_UP) {
        plyr.update(scrRend, MOVED_UP, enmy, &meteorsArr);
        noMove = false;
    }

    if (noMove) {
        plyr.update(scrRend, NO_MOVEMENT, enmy, &meteorsArr);
    }
}

/**************************
 *   METHODS FOR METEORS  *
 *************************/

void GameInstance :: updateMeteors() {
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
}

void GameInstance :: drawMeteors() {
    for (int i = meteorsArr.getSize() - 1; i >= 0 ; i--) {
            meteorsArr[i].drawShape(scrRend);
    }
}

/**************************
 *   GENERATION/ REMOVAL  *
 *************************/

void GameInstance :: playerGenerateBullet() {
    plyr.generateBullet();
}

void GameInstance :: enemyGenerateBullet() {
    static unsigned long bullet_time = 0;

    if (millis() - bullet_time >= bulletSpawnTime) {
        bullet_time = millis();
        enmy->generateBullet();
    }
}

void GameInstance :: deleteEnemy() {
    if (enmy) {
        enmy->clearBullets(scrRend);
        enmy->clearShape(scrRend);

        delete enmy;
        enmy = NULL;
    }
}

void GameInstance :: deleteMeteors() {
    for (int i = meteorsArr.getSize() - 1; i >= 0; i--) {
            meteorsArr[i].clearShape(scrRend);
            meteorsArr.remove(i);
    }
}

void GameInstance :: generateMeteor() {
    static unsigned long last_spawn_time = 0;

    if (millis() - last_spawn_time >= meteorSpawnTime) {
        last_spawn_time = millis();

        byte randCoordX = random(1, MAX_X);

        Entity *eRef = new Entity(METEOR, DOWN, randCoordX, INITIAL_METEORY, METEOR_TOPX, METEOR_TOPY);

        eRef->setSpeed(speed);

        meteorsArr.pushBack(eRef);
    }
}

void GameInstance :: generateEnemy() {
    byte randNumber = random(1, NO_OF_ENEMIES + 1);

    if (randNumber == 1)
        enmy = new
            Enemy(plyr, ENEMY1, DOWN, 2, 1, ENEMY1_TOPX, ENEMY1_TOPY, ENEMY1_LEFTX, ENEMY1_RIGHTX, ENEMY1_BOTTOMY, ENEMY_HEALTH);
    if (randNumber == NO_OF_ENEMIES)
        enmy = new
            Enemy(plyr, ENEMY2, DOWN, 2, 1, ENEMY2_TOPX, ENEMY2_TOPY, ENEMY2_LEFTX, ENEMY2_RIGHTX, ENEMY2_BOTTOMY, ENEMY_HEALTH);

    enmy->setSpeed(speed);
}
