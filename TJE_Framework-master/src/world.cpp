//
//  world.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "world.h"

World* World::instance = NULL;

World::World() {
    instance = this;
    this->player = Player();
}

void saveWorld() {
    //save player, enemies (positions, rotations...)
}
void loadWorld() {
    //load player, enemies (positions, rotations...)
}

void AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture, int window_width, int window_height) {
    Vector2 mousePos = Input::mouse_position;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, window_width, window_height);
    Vector3 rayOrigin = cam->eye;

    Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
    Matrix44 model;
    model.translate(spawnPos.x, spawnPos.y, spawnPos.z);

    Entity* entity = new Entity(model, mesh, texture);
    //entities.push_back(entity);
}
