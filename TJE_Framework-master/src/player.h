//
//  player.h
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#ifndef player_h
#define player_h

#include "utils.h"
#include "includes.h"
#include "camera.h"
#include "entity.h"
#include "bullet.h"


class Player {
public:
    static Player* instance;
    Vector3 pos;
    float yaw;
    float pitch;
    Vector2 max_pitch;
    bool shooting;
    float gunAngle;
    bool shot;
    bool gunUp;
    Vector3 character_center;
    bool look;
    bool collidingWithEntities;
    bool collidingWithEnemies;
    float shoot_cooldown;
    bool enemy;
    unsigned int id;
    float bullet_offset;

    Player(unsigned int id);
    Matrix44 getModel();
    void Shoot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, Matrix44 playerModel, Player* player);
    Matrix44 Coil(float elapsed_time, Matrix44 gun);
    void AIEnemy(float elapsed_time, Player* player, std::vector<Entity*> entities, std::vector<Entity*> enemies, bool cameraLocked);
    void checkColisions(Vector3 playerVel, std::vector<Entity*> entities, float elpased_time, float radio_coll);
    void setSpawnPoint(int currentLevel);
};



#endif /* player_h */
