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
struct sBullet {
    Vector3 pos;
    Vector3 vel;
}; 
sBullet bullet;

Player::Player() {
    instance = this;
    shooting = false;
}

void Player::Shot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    Mesh* mesh_bullet = Mesh::Get("data/sphere.obj");
    Matrix44 model;
    model.scale(0.05, 0.05, 0.05);
    Texture* texture_bullet = g->texture_black; //la textura de la bala es tota negra
    bullet.pos = this->pos + Vector3(0, 20 , -5);
    model.translate(bullet.pos.x, bullet.pos.y, bullet.pos.z);
    //RENDER BULLET MESH, es podria fer una funcio RENDER MESH i que la cridi aqui i dins render entity...
    float time = Game::instance->time;
    //enable shader

    a_shader->enable();

    //upload uniforms
    a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
    a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
    if (texture_bullet != NULL) {
        a_shader->setUniform("u_texture", texture_bullet, 0);
    }
    a_shader->setUniform("u_time", time);
    //a_shader->setUniform("u_tex_tiling", tiling);
    a_shader->setUniform("u_model", model);
    mesh_bullet->render(primitive);

    a_shader->disable();

    if (!cameraLocked) {
        mesh_bullet->renderBounding(model);
    }
    //END RENDER MESH BULLET
    //bullet.pos = bullet.pos + (bullet.vel * g->elapsed_time);
}