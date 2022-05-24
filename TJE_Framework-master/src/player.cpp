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
//struct sBullet {
//    Vector3 pos;
//    int vel = 100.f;
//}; 
//sBullet bullet;

Player::Player() {
    instance = this;
    shooting = false;
    this->pitch = 0.0f;
    this->yaw = 0.0f;
}

std::vector<Entity*> Player::Shot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, std::vector<Entity*> entities) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);

    Mesh* mesh_bullet = Mesh::Get("data/sphere.obj");
    Matrix44 model;
    model.scale(0.05, 0.05, 0.05);
    Texture* texture_bullet = g->texture_black; //la textura de la bala es tota negra
    
    Vector3 position = this->pos + Vector3(0.0f, 0.5f, 0.0f); //inicialitzem la posicio de la bala devant del PLAYER

    model.setTranslation(position.x, position.y, position.z);

    Entity* entity_bullet = new Entity(model, mesh_bullet, texture_bullet);
    entity_bullet->current_entity = Entity::ENTITY_ID::BULLET;
    entity_bullet->dir = dir;

    entities.push_back(entity_bullet);
    return entities;
}