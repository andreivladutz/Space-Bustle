#ifndef GAMEINSTANCE_H_INCLUDED
#define GAMEINSTANCE_H_INCLUDED

#include "Actor.h"
#include "screenRenderer.h"
#include "controlInterface.h"
#include "entityArray.h"

#define METEOR_SHOWER 0
#define ENEMY_FIGHT 1
#define GAME_PAUSED 2
#define GAME_OVER 3

#define UPDATE_INTERVAL 100

#define LEVEL1_METEOR_SHOWER 10000
#define LEVEL1_BULLET_SPAWN 2000
#define LEVEL1_METEOR_SPAWN 2000

#define SPEED_INCREASE_LEVEL_INTERVAL 2

#define BULLET_SPAWN_DECREASE_TIME 50
#define METEOR_SPAWN_DECREASE_TIME 50

#define INITIAL_PLAYERX 4
#define INITIAL_PLAYERY 5
#define INITIAL_METEORY 0

class GameInstance {
    ScreenRenderer scrRend;
    ControlInterface cI;
    Player plyr;
    Enemy *enmy;
    EntityArray meteorsArr;

    unsigned long meteorShowerDuration, meteorSpawnTime, bulletSpawnTime, showerStartTime;

    byte gameState, lastGameState, level, speed;
public :
    GameInstance();

    //all drawing and position updating logic
    void update();

    void updateGameObjects();
    void drawGameObjects();

    //update position. check collision with player
    void updateMeteors();
    void drawMeteors();

    //entity spawning
    void playerGenerateBullet();
    void enemyGenerateBullet();
    void generateMeteor();
    void generateEnemy();
    //cleanup for meteors and enemy
    void deleteEnemy();
    void deleteMeteors();

    //switch between game states
    //doing cleanup and initializing
    void checkGameState();
    void increaseLevel();
};


#endif // GAMEINSTANCE_H_INCLUDED
