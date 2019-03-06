#include "gameInstance.h"

GameInstance *thisInstance;

void onJoystickPress() {
    static unsigned long last_call_time = millis();

    if (millis() - last_call_time >= DELTA_TIME) {
        last_call_time = millis();
        thisInstance->playerGenerateBullet();
    }
}

void onGameButtonPress() {
    static unsigned long last_call_time = 0;

    if (millis() - last_call_time >= DELTA_TIME * 3) {
        last_call_time = millis();

        //resuming game
        if (thisInstance->gameState == GAME_PAUSED) {
            thisInstance->pauseTime = millis() - thisInstance->pauseStartTime;

            thisInstance->showerStartTime += thisInstance->pauseTime;

            thisInstance->gameState = thisInstance->lastGameState;
            thisInstance->scrRend.LCDForcedUpdate(
                                                  thisInstance->plyr.getLife(),
                                                  thisInstance->level,
                                                  thisInstance->score
                                                  );
        }
        else if (thisInstance->gameState == GAME_OVER) {
            thisInstance->reinitGame();
        }
        //pausing game
        else {
            thisInstance->pauseStartTime = millis();
            thisInstance->lastGameState = thisInstance->gameState;

            //reinit time so it doesn't spawn bullets/meteors instantly after resume
            thisInstance->bullet_time = millis();
            thisInstance->meteor_time = millis();

            thisInstance->gameState = GAME_PAUSED;
            thisInstance->scrRend.LCDPrintMultiLineMessage("GAME PAUSE.PRESS", "BUTTON TO RESUME");
        }
    }
}

/*******************************************
 *   GAME INITIALISING AND REINITIALISING  *
 ******************************************/

GameInstance :: GameInstance()
        : plyr(MAIN, UP, INITIAL_PLAYERX, INITIAL_PLAYERY, MAIN_TOPX, MAIN_TOPY, MAIN_LEFTX, MAIN_RIGHTX, MAIN_BOTTOMY, PLAYER_HEALTH),
        enmy(NULL), meteorShowerDuration(LEVEL1_METEOR_SHOWER), meteorSpawnTime(LEVEL1_METEOR_SPAWN),
        bulletSpawnTime(LEVEL1_BULLET_SPAWN), showerStartTime(millis()), meteor_time(0), bullet_time(0), score(0), pauseStartTime(0), pauseTime(0),
        gameState(GAME_PAUSED), lastGameState(METEOR_SHOWER), level(1), speed(CONSTANT_SPEED), pauseTimeUsed(0)
{
    randomSeed(analogRead(0));

    thisInstance = this;
    cI.setJoystickClickListener(onJoystickPress);
    cI.setGameButtonClickListener(onGameButtonPress);

    scrRend.LCDPrintMultiLineMessage("PRESS BUTTON TO", "   BEGIN GAME");
}

void GameInstance :: reinitGame() {
    deleteEnemy();
    deleteMeteors();

    plyr.clearShape(scrRend);
    plyr.setPosX(INITIAL_PLAYERX);
    plyr.setPosY(INITIAL_PLAYERY);
    plyr.setLife(PLAYER_HEALTH);

    meteorShowerDuration = LEVEL1_METEOR_SHOWER;
    meteorSpawnTime = LEVEL1_METEOR_SPAWN;
    bulletSpawnTime = LEVEL1_BULLET_SPAWN;
    showerStartTime = millis();
    score = 0;
    pauseStartTime = 0;
    pauseTime = 0;
    gameState = METEOR_SHOWER;
    lastGameState = METEOR_SHOWER;
    level = 1;
    speed = CONSTANT_SPEED;
    pauseTimeUsed = 0;

    meteor_time = millis();

    scrRend.LCDForcedUpdate(plyr.getLife(), level, score);
}

/*******************************
 *   GAME LOGIC AND RENDERING  *
 ******************************/

void GameInstance :: update() {
    static bool last_drew = false;
    static unsigned long last_draw_time;

    checkGameState();

    if (gameState == GAME_PAUSED || gameState == GAME_OVER)
        return;

    increaseScore();

    if (!last_drew) {
        last_draw_time = millis();
        last_drew = true;

        drawGameObjects();

        scrRend.updateScreen();
        scrRend.LCDUpdate(plyr.getLife(), level, score);
    }

    if (last_drew && millis() - last_draw_time >= UPDATE_INTERVAL) {
        last_drew = false;

        updateGameObjects();
    }

}

void GameInstance :: checkGameState() {
    if (gameState != GAME_OVER && plyr.getLife() <= 0) {
        gameState = GAME_OVER;
        deleteEnemy();
        deleteMeteors();

        scrRend.LCDPrintMessage("GAME OVER.RETRY?");
        scrRend.LCDPrintScore(score);
    }

    //if we defeated the enemy we increase the level and difficulty
    else if (gameState == ENEMY_FIGHT && enmy->getLife() <= 0) {
        score += ENEMY_DEFEAT_SCORE * level;
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

    meteorShowerDuration += SECOND * level;

    if (bulletSpawnTime < BULLET_SPAWN_DECREASE_TIME * level)
        bulletSpawnTime = MIN_BULLET_SPAWN_TIME;
    else
        bulletSpawnTime = bulletSpawnTime - BULLET_SPAWN_DECREASE_TIME;

    //we increase speed every two levels
    //with a slow growing function
    if (level % SPEED_INCREASE_LEVEL_INTERVAL == 0)
        speed = max(int(CONSTANT_SPEED * sqrt(level)), CONSTANT_SPEED);
}

void GameInstance :: increaseScore() {
    static unsigned long last_increased = 0;

    if (millis() - last_increased >= 3 * SECOND) {
        last_increased = millis();
        score++;
    }
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
    if (millis() - meteor_time >= meteorSpawnTime) {
        meteor_time = millis();

        byte randCoordX = random(1, MAX_X);

        Entity *eRef = new Entity(METEOR, DOWN, randCoordX, INITIAL_METEORY, METEOR_TOPX, METEOR_TOPY);

        eRef->setSpeed(speed);

        meteorsArr.pushBack(eRef);
    }
}

void GameInstance :: generateEnemy() {
    byte randNumber = random(1, NO_OF_ENEMIES + 1), randomPosX = random(1, MAX_X - 1);

    if (randNumber == 1)
        enmy = new
            Enemy(plyr, ENEMY1, DOWN, randomPosX, INITIAL_ENEMYY, ENEMY1_TOPX, ENEMY1_TOPY, ENEMY1_LEFTX, ENEMY1_RIGHTX, ENEMY1_BOTTOMY, ENEMY_HEALTH);
    if (randNumber == NO_OF_ENEMIES)
        enmy = new
            Enemy(plyr, ENEMY2, DOWN, randomPosX, INITIAL_ENEMYY, ENEMY2_TOPX, ENEMY2_TOPY, ENEMY2_LEFTX, ENEMY2_RIGHTX, ENEMY2_BOTTOMY, ENEMY_HEALTH);

    enmy->setSpeed(speed);

    bullet_time = millis();
}
