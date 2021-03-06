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

Player::Player(unsigned int id) {
    instance = this;
    shooting = false;
    this->pitch = 0.0f;
    this->yaw = 0.0f;
    gunAngle = 0.0f;
    shot = false;
    gunUp = true;
    look = false;
    collidingWithEntities = false;
    collidingWithEnemies = false;
    shoot_cooldown = 0.0f;
    enemy = true;
    this->id = id;
    bullet_offset = 0.5f;
    max_pitch.x = -40.0f;
    max_pitch.y = 40.0f;

}

Matrix44 Player::getModel() {
    Matrix44 model;
    model.translate(pos.x, pos.y, pos.z);
    model.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
    return model;
}

void Player::Shoot(int primitive, Camera* cam, Shader* a_shader, bool cameraLocked, Matrix44 playerModel, Player* player) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    World* w = World::instance;
    Bullet*& bullet = w->Bullets[w->GetFreeBullet()];
    
    Vector3 dir;
    if (enemy) {
        int random = (rand() % 3) - 1;
        dir = (player->pos + Vector3(0.01f * random, 0.01f* random, 0.01f* random) - this->pos).normalize(); //Apliquem un petit offset random a nes Vector3 de la posicio del jugador!
    }
    else
        dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    
    Matrix44 model;
    
    bullet->author = this->id;
    bullet->isActive = true;
    float positionY = this->pos.y + 0.5f; //inicialitzem la posicio de la bala devant del PLAYER
    if (!enemy) { bullet_offset = 0.2;} //si es tracta del Jugador, el offset de on surt la BALA, que sigui mes próxim a la pistola
    model.setTranslation(playerModel.getTranslation().x + dir.x* bullet_offset, positionY, playerModel.getTranslation().z+ dir.z * bullet_offset);

    model.rotate(this->yaw * DEG2RAD, Vector3(0, 1, 0));
    bullet->entity_bullet->model = model;

    bullet->entity_bullet->dir = dir;
    bullet->entity_bullet->yaw = this->yaw;

    g->ChShoot = g->PlayGameSound(g->shoot);
    //g->ChRecoil = g->PlayGameSound(g->recoil); //de moment el deix comentat perque van massa seguits els dos audios i no m'acaba de molar
}

Matrix44 Player::Coil(float elapsed_time, Matrix44 gun) {
    
    if (gunUp) {
        gunAngle += 1000.0f * elapsed_time;
    }
    if (gunAngle >= 26.0f) {
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


void Player::AIEnemy(float elapsed_time, Player* player, std::vector<Entity*> entities, std::vector<Entity*> enemies, bool cameraLocked) {
    float facingDistance = 9.0f; //distancia la qual ens comença a veure un enemic
    Game* g = Game::instance;
    Matrix44 model = this->getModel();    
    Vector3 side = model.rotateVector(Vector3(1, 0, 0)).normalize();
    Vector3 forward = model.rotateVector(Vector3(0, 0, -1)).normalize();

    Vector3 toTarget = player->pos - this->pos;
    float dist = toTarget.length();
    toTarget.normalize();

    float sideDot = side.dot(toTarget);
    float forwardDot = forward.dot(toTarget);
    if (dist < facingDistance) { //si esta lluny no sa encari cap al jugador
        
        if (forwardDot < 0.98f) { //pq no intenti encarar-se més si ja esta casi perfectament encarat
            yaw += 90.0f * g->GetCurrent()->world.sign(sideDot) * elapsed_time*2.0f; //el *2 es perque la rotacio es molta lenta en comparació el desplaçament
        }
        else {
            if (dist > 2.0f) { //que no s'atraqui més de 2 unitats 
                Vector3 playerVel = forward * 1.0f * elapsed_time;
                this->checkColisions(playerVel, entities, elapsed_time, 0.4f); //abans de canviar la posicio mira si colisiona amb entitats
                this->checkColisions(playerVel, enemies, elapsed_time, 0.03f); // i enemics

                //if (this->collidingWithEntities) {
                    //pos = pos + Vector3(0.02f, 0.0f, 0.0f);

                //}
                if (this->collidingWithEnemies) { //si hi ha colisio enemic-enemic
                    int random = (rand() % 3)- 1;
                    pos = pos + Vector3(0.01f * random, 0.0f, 0.01f * random); //apliquem desplaçament random entre -1 0 1
                }

            }
            shoot_cooldown += elapsed_time;
            if (shoot_cooldown > 1) {
                shoot_cooldown = 0.0f;//reiniciem cooldown, cada enemic pot disparar cada 1 segon
                Shoot(GL_TRIANGLES, g->camera, g->shader, cameraLocked, model, player); //enemics disparen
            }
        }
        look = true;
    }
    else{ look = false; }
    
}
void Player::checkColisions(Vector3 playerVel, std::vector<Entity*> entities, float elpased_time, float radio_coll) {
    Game* g = Game::instance;
    Vector3 nexPos = pos + playerVel;
    //calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
    character_center = nexPos + Vector3(0, 0.5, 0);
    
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* currentEntity = entities[i];

        if (currentEntity->current_entity == Entity::ENTITY_ID::ENEMY && i == this->id) { //si l'entitat és un enemic i és ell mateix
            continue;
        }
        Vector3 coll;
        Vector3 collnorm;
        Matrix44 current_model; //creem una model per fer els càlculs, pq si es enemic, no utilitzarem la seva model!
        Mesh* current_mesh; //creem una mesh per la mateixa raó
        if (currentEntity->current_entity == Entity::ENTITY_ID::ENEMY) {
            //Si la entitat es un enemic, agafa model nomes amb propietats de posicio, i MESH de un RECTANGLE de la seva mida
            current_model.setTranslation(currentEntity->model.getTranslation().x, currentEntity->model.getTranslation().y, currentEntity->model.getTranslation().z);
            current_mesh = g->box_col;
        }
        else {
            current_model = currentEntity->model;
            current_mesh = currentEntity->mesh;
        }
        //comprobamos si colisiona el objeto con la esfera
        if (!currentEntity->mesh->testSphereCollision(current_model, character_center, radio_coll, coll, collnorm)) {

            if (currentEntity->current_entity == Entity::ENTITY_ID::ENEMY){ //si colisiona amb un altra enemic
                this->collidingWithEnemies = false;
            }
            else {
                this->collidingWithEntities = false;

            }
            continue; //si no colisiona, pasamos al siguiente objeto
        }
        //si la esfera est‡ colisionando muevela a su posicion anterior alejandola del objeto
        Vector3 push_away = normalize(coll - character_center) * elpased_time;
        nexPos = pos - push_away; //move to previous pos but a little bit further
        //cuidado con la Y, si nuestro juego es 2D la ponemos a 0
        nexPos.y = 0;

        if (currentEntity->current_entity == Entity::ENTITY_ID::ENEMY) {
            this->collidingWithEnemies = true;
        }
        else {
            this->collidingWithEntities = true;

        }
        break;
    }
    pos = nexPos;
    
}
void Player::setSpawnPoint(int currentLevel) {
    Vector3 level0 = { -4.0f, 0.0f, -10.5f };
    Vector3 level1 = { -0.95f, 0.0f, 1.0f };
    Vector3 level2 = { -0.05f, 0.0f, 3.04f };

    std::vector<Vector3> spawnPointsLevels;
    spawnPointsLevels.push_back(level0);
    spawnPointsLevels.push_back(level1);
    spawnPointsLevels.push_back(level2);


    pos = spawnPointsLevels[currentLevel];
    yaw = 90.0f;
}
