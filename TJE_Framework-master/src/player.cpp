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
    shot = false;
    gunUp = true;
    look = false;
    colliding = false;
    shoot_cooldown = 0.0f;
    enemy = true;
}

Matrix44 Player::getModel() {
    Matrix44 model;
    model.translate(pos.x, pos.y, pos.z);
    model.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
    return model;
}

std::vector<Entity*> Player::Shoot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, std::vector<Entity*> entities, Matrix44 playerModel) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir;
    if (enemy)
        dir = g->player->pos - this->pos;
    else
        dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    
    Mesh* mesh_bullet = g->mesh_bullet;
    Matrix44 model;
    //model.scale(0.01, 0.01, 0.01);
    Texture* texture_bullet = g->texture_bullet; //la textura de la bala es tota negra
    
    float positionY = this->pos.y + 0.5f; //inicialitzem la posicio de la bala devant del PLAYER
    
    model.setTranslation(playerModel.getTranslation().x + dir.x*0.2, positionY, playerModel.getTranslation().z+ dir.z * 0.2);
    model.rotate(this->yaw * DEG2RAD, Vector3(0, 1, 0));

    Entity* entity_bullet = new Entity(model, mesh_bullet, texture_bullet);
    entity_bullet->current_entity = Entity::ENTITY_ID::BULLET;
    entity_bullet->dir = dir;

    entities.push_back(entity_bullet);
    g->PlayGameSound(g->shoot);
    return entities;
}

Matrix44 Player::Coil(float elapsed_time, Matrix44 gun) {
    
    if (gunUp) {
        gunAngle += 1000.0f * elapsed_time;
    }
    if (gunAngle >= 22.0f) {
        gunUp = false;    
    }

    if(!gunUp){
        gunAngle -= 1000.0f * elapsed_time;
    }
    gun.rotate(-gunAngle * DEG2RAD, Vector3(1, 0, 0) * elapsed_time);

    if(gunAngle <= 0.0f){
        gunAngle = 0.0f;
        this->gunUp = true;
        this->shot = false;
    }
    
    return gun;
}


void Player::AIEnemy(float elapsed_time) {
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
        
        if (forwardDot < 0.98f && !colliding) { //pq no intenti encarar-se més si ja esta casi perfectament encarat
            yaw += 90.0f * g->world.sign(sideDot) * elapsed_time;
        }
        else {
            if (dist > 2.0f) { //que no s'atraqui més de 2 unitats 
                Vector3 playerVel = forward * 3.0f * elapsed_time;
                this->checkColisions(playerVel, g->entities, elapsed_time); //abans de canviar la posicio mira si colisiona
                if (this->colliding) {
                    pos = pos + Vector3(0.02f, 0.0f, 0.0f);
                }
            }
            shoot_cooldown += elapsed_time;
            if (shoot_cooldown > 1) {
                shoot_cooldown = 0.0f;//reiniciem cooldown, cada enemic pot disparar cada 1 segon
                g->bullets = Shoot(GL_TRIANGLES, g->camera, g->shader, g->cameraLocked, g->bullets, model); //enemics disparen
            }
        }
        look = true;
    }
    else{ look = false; }
    
}
void Player::checkColisions(Vector3 playerVel, std::vector<Entity*> entities, float elpased_time) {
    Vector3 nexPos = pos + playerVel;
    //calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
    character_center = nexPos + Vector3(0, 0.5, 0);
    
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* currentEntity = entities[i];

        Vector3 coll;
        Vector3 collnorm;
        //comprobamos si colisiona el objeto con la esfera
        if (!currentEntity->mesh->testSphereCollision(currentEntity->model, character_center, 0.2, coll, collnorm)) {
            this->colliding = false;
            continue; //si no colisiona, pasamos al siguiente objeto
        }
        //si la esfera est‡ colisionando muevela a su posicion anterior alejandola del objeto
        Vector3 push_away = normalize(coll - character_center) * elpased_time;
        nexPos = pos - push_away; //move to previous pos but a little bit further
        //yaw = yaw + 10.0f;
        //cuidado con la Y, si nuestro juego es 2D la ponemos a 0
        nexPos.y = 0;
        this->colliding = true;
        /*float coll_magnitud = sqrt(pow(coll.x, 2) + pow(coll.y, 2) + pow(coll.z, 2));
        float collnorm_magnitud = sqrt(pow(collnorm.x, 2) + pow(collnorm.y, 2) + pow(collnorm.z, 2));
        yaw = acos(dot(coll, collnorm) / coll_magnitud * collnorm_magnitud);*/

        break;
        //reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
        //playerVel = reflect(playerVel, collnorm) * 0.95;
    }
    pos = nexPos;
    
}

