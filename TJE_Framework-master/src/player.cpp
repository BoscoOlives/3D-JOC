//
//  player.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "player.h"
#include "game.h"
#include "input.h"


Player* Player::instance = NULL;

Player::Player() {
    instance = this;
    shooting = false;
    this->pitch = 0.0f;
    this->yaw = 0.0f;
    gunAngle = 0.0f;
    shoot = false;
    gunUp = true;
}

Matrix44 Player::getModel() {
    Matrix44 model;
    model.translate(pos.x, pos.y, pos.z);
    model.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
    return model;
}

std::vector<Entity*> Player::Shot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, std::vector<Entity*> entities) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);

    Mesh* mesh_bullet = g->mesh_bullet;
    Matrix44 model;
    //model.scale(0.01, 0.01, 0.01);
    Texture* texture_bullet = g->texture_bullet; //la textura de la bala es tota negra
    
    Vector3 position = this->pos + Vector3(0.0f, 0.5f, 0.0f); //inicialitzem la posicio de la bala devant del PLAYER

    model.setTranslation(position.x, position.y, position.z);

    model.rotate(this->yaw * DEG2RAD, Vector3(0, 1, 0));

    Entity* entity_bullet = new Entity(model, mesh_bullet, texture_bullet);
    entity_bullet->current_entity = Entity::ENTITY_ID::BULLET;
    entity_bullet->dir = dir;

    entities.push_back(entity_bullet);
    return entities;
}

Matrix44 Player::Coil(float elapsed_time, Matrix44 gun) {
    
    if (gunUp) {
        gunAngle += 1000.0f * elapsed_time;
    }
    if (gunAngle >= 45.0f) {
        gunUp = false;    
    }

    if(!gunUp){
        gunAngle -= 1000.0f * elapsed_time;
    }
    gun.rotate(-gunAngle * DEG2RAD, Vector3(1, 0, 0) * elapsed_time);

    if(gunAngle <= 0.0f){
        gunAngle = 0.0f;
        this->gunUp = true;
        this->shoot = false;
    }
    
    return gun;
}


void Player::AIEnemy(float elpased_time) {
    float facingDistance = 10.0f;
    Game* g = Game::instance;
    Matrix44 model = this->getModel();    
    Vector3 side = model.rotateVector(Vector3(1, 0, 0)).normalize();
    Vector3 forward = model.rotateVector(Vector3(0, 0, -1)).normalize();

    Vector3 toTarget = g->player->pos - this->pos;
    float dist = toTarget.length();
    toTarget.normalize();

    float sideDot = side.dot(toTarget);
    float forwardDot = forward.dot(toTarget);
    if (dist < facingDistance) { //si esta lluny no sa encari cap al jugador
        if (forwardDot < 0.98f) { //pq no intenti encarar-se més si ja esta casi perfectament encarat
            yaw += 90.0f * g->world.sign(sideDot) * elpased_time;
        }
        if (dist > 4.0f) { //que no s'atraqui més de 4 unitats 
            this->pos = this->pos + (forward * 10.0f * elpased_time);
        }
    }

}