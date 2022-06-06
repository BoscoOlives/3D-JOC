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


class Player {
public:
    static Player* instance;
    Vector3 pos;
    float yaw;
    float pitch;
    bool shooting;
    float gunAngle;
    bool shot;
    bool gunUp;
    Vector3 character_center;
    bool look;

    Player();
    Matrix44 getModel();
    std::vector<Entity*> Shoot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, std::vector<Entity*> entities);
    Matrix44 Coil(float elapsed_time, Matrix44 gun);
    void AIEnemy(float seconds_elapsed, float elapsed_time);
    void checkColisions(Vector3 playerVel, std::vector<Entity*> entities, float elpased_time);
};



#endif /* player_h */
