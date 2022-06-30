//
//  world.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "world.h"
#include "Game.h"
#include <iostream>
#include <fstream>


World* World::instance = NULL;


World::World() {
    instance = this;   
    printf("Constructor WORLD\n");
    
}

void World::saveEnemies(char* path) {
    //save player, enemies (positions, rotations...)
    printf("Saving Enemies in World...\n");
    FILE* file = fopen(path, "wb");
    
    for (size_t i = 0; i < enemies.size(); i++)
    {
        Entity* entity = enemies[i];
        Matrix44 model = entity->model;

        if (entity->current_entity != Entity::ENTITY_ID::BULLET) {
            std::string str = {
            std::to_string(entity->current_entity) + "\n" +
            std::to_string(model._11) + "\n" + std::to_string(model._12) + "\n" + std::to_string(model._13) + "\n" + std::to_string(model._14) + "\n" +
            std::to_string(model._21) + "\n" + std::to_string(model._22) + "\n" + std::to_string(model._23) + "\n" + std::to_string(model._24) + "\n" +
            std::to_string(model._31) + "\n" + std::to_string(model._32) + "\n" + std::to_string(model._33) + "\n" + std::to_string(model._34) + "\n" +
            std::to_string(model._41) + "\n" + std::to_string(model._42) + "\n" + std::to_string(model._43) + "\n" + std::to_string(model._44) + "\n" };

            const char* buffer = str.c_str();

            fwrite(buffer, sizeof(char), strlen(buffer), file);
        }
    }
    fclose(file);
}
void World::loadEnemies(char* path) {
    //load player, enemies (positions, rotations...)
    Game* g = Game::instance;
    Mesh* mesh;
    Texture* texture;
    int id;
    std::string line;
    std::ifstream myfile(path);

    Matrix44 model;
    if (myfile.is_open())
    {
        int i = 0;
        while (getline(myfile, line))
        {
            if (i == 0) {
                id = std::stoi(line);
            }
            else if (i>0 && i<17) { //16 valors de la MODEL matrix44
                model.m[i-1] = std::stof(line); //cast de string a float
            }
            if (i == 16) { //quan es porten 16 valors de una matriu, es dona per suposat que es troba una nova MODEL
                this->get_Mesh_Texture_Entity(id, mesh, texture); //funció per obtenir la mesh i la texture depenent de la ID
                Entity* entity = new Entity(model, mesh, texture);
                entity->current_entity = (Entity::ENTITY_ID)id;

                if(entity->current_entity == Entity::ENTITY_ID::ENEMY){
                    
                    Player* player = new Player((unsigned int)enemies.size()+1); //declarem un nou Player amb el seu ID corresponent (ID enemics 1,2,..,n)
                    player->pos = model.getTranslation(); //guardem la posicio a partir de la informació de la model
                    player_enemies.push_back(player);//add del enemic a la llista de players
                    enemies.push_back(entity);//add del enemic a la llista de entities
                }
                else{
                    entities.push_back(entity);
                }
                i = -1;
            }
            //std::cout << line << "\n";
            i = i+1;
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";

    printf("\nLoading Enemies...\n");

}

void World::loadWorld(char* path) {
    std::string STR = "";
    readFile(path, STR);
    std::stringstream ss(STR);

    while (!ss.eof()) {
        std::string mPath;
        std::string tPath;
        ss >> mPath;
        ss >> tPath;

        //const char* meshPathChar = &mPath[0];
        //+-const char* texturePathChar = &tPath[0];

        Matrix44 model;
        for (size_t i = 0; i < 16; i++)
        {
            ss >> model.m[i];
            if (ss.peek() == ',') ss.ignore();
        }

        Mesh* mesh = Mesh::Get(&mPath[0]);
        Texture* texture = Texture::Get(&tPath[0]);
        
        Entity* entity = new Entity(model, mesh, texture);
        entities.push_back(entity);

    }
    
}



void World::get_Mesh_Texture_Entity(int id, Mesh* &mesh, Texture* &texture) {//funció per obtenir la mesh i la texture depenent de la ID
    Game* g = Game::instance;


    if (id == Entity::ENTITY_ID::BARREL) {
        mesh = g->mesh_barrel;
        texture = g->texture_barrel;
    }
    else if (id == Entity::ENTITY_ID::CONSOLE) {
        mesh = g->mesh_consoleScreen;
        texture = g->texture_consoleScreen;
    }
    else if (id == Entity::ENTITY_ID::SUPPORT) {
        mesh = g->mesh_SupportCorner;
        texture = g->texture_SupportCorner;
    }
    else if (id == Entity::ENTITY_ID::ENEMY) {
        mesh = g->mesh_cowboy_run;
        texture = g->texture_cowboy;
    }
    else if (id == Entity::ENTITY_ID::ROCK1) {
        mesh = g->mesh_rock1;
        texture = g->texture_rock1;
    }
}

void World::AddEntityInFront(Camera* cam, Entity::ENTITY_ID entityToAdd) {
    Game* g = Game::instance;
    Mesh* mesh;
    Texture* texture;
    this->get_Mesh_Texture_Entity(entityToAdd, mesh, texture);//funció per obtenir la mesh i la texture depenent de la ID

    Vector2 mousePos = Input::mouse_position;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
    Matrix44 model;
    model.translate(spawnPos.x, spawnPos.y, spawnPos.z);
    if (entityToAdd == Entity::ENTITY_ID::ENEMY){
        model.scale(0.01, 0.01, 0.01);
        Player* player = new Player((unsigned int)enemies.size()+1);
        player->pos = spawnPos;
        player_enemies.push_back(player);
        Entity* entity = new Entity(model, mesh, texture);
        entity->current_entity = (Entity::ENTITY_ID)entityToAdd;

        enemies.push_back(entity);
    }
    else {
        Entity* entity = new Entity(model, mesh, texture);
        entity->current_entity = (Entity::ENTITY_ID)entityToAdd;

        entities.push_back(entity);
    }
}

Entity* World::RayPick(Camera* cam, Entity* selectedEntity, float max_ray_dist) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    for (size_t i = 0; i < this->entities.size(); i++)
    {
        Entity* entity = this->entities[i];
        Vector3 pos;
        Vector3 normal;
        //testRayCollision(Matrix44 model, Vector3 start, Vector3 front, Vector3& collision, Vector3& normal, float max_ray_dist, bool in_object_space )
        if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal, max_ray_dist)) {

            selectedEntity = entity;
            //if (selectedEntity == NULL) printf("selectedEntity NOT WORKING\n");
            printf("selectedEntity\n");
            break;
        }
    }
    return selectedEntity;
}

void World::RotateSelected(float angleDegrees, Entity* selectedEntity)
{
    if (selectedEntity == NULL)
    {
        printf("selectedEntity = NULL\n");
        return;
    }
    selectedEntity->model.rotate(angleDegrees * DEG2RAD, Vector3(0, 1, 0));
    printf("rotating %f degrees\n", angleDegrees);
}

void World::DeleteEntity(Camera* cam) {
    
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    for (int i = (int)entities.size() - 1; i >= 0; i--)
    {
        Entity* entity = entities[i];
            Vector3 pos;
            Vector3 normal;
        if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
            entities.erase(entities.begin() + i);
            printf("Entity Removed\n");
            printf("ID: %d\n", i+1);    //es correspona  la linia del txt de carga de mapa!
            return;
        }
    }

    for (int i = (int)enemies.size() - 1; i >= 0; i--)
    {
        Entity* entity = enemies[i];
        Vector3 pos;
        Vector3 normal;
        Matrix44 current_model;
        //en cas de ser un enemic, agafem la seva posicio i cap propietat més de la model
        current_model.setTranslation(entity->model.getTranslation().x, entity->model.getTranslation().y, entity->model.getTranslation().z);
        if (g->box_col->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
            enemies.erase(enemies.begin() + i);
            player_enemies.erase(player_enemies.begin() + i);//si l'esfera col·lisiona, elimina al player enemic
            printf("Entity Removed\n");
            return;
        }
    }

    printf("No Entity Removed\n");
     
}

void World::shooting_update(Entity*& entityPlayer, std::vector<char*> levelsWorld, std::vector<char*> levelsEnemies, int currentLevel) {
    // FOR LOOP PER FER UPDATE DE LA POSICIÓ DE LA BALA
    Game* g = Game::instance;
    for (size_t i = 0; i < numBullets; i++) //bullets.size
    {
        Bullet*& currentBullet = Bullets[i];
        if (!currentBullet->isActive) {
            continue;
        }
        Entity* entity = currentBullet->entity_bullet; // cercam les BULLETS
        float vel = 5.0f;
        entity->update_position_moving(g->elapsed_time, vel);
            
        Vector3 bullet_center = entity->model.getTranslation();
        if (bullet_center.y < 0.0f) { //si la bala atravessa el terra, elimina la bala
            currentBullet->isActive = false;
            //bullets.erase(bullets.begin() + i);
            continue;
        }
        Vector3 delete_dist = bullet_center - player.pos;
        if (delete_dist.length() > 20.0f) { //si la bala es massa lluny, elimina la bala
            currentBullet->isActive = false;
            //bullets.erase(bullets.begin() + i);
            continue;
        }

        for (int j = (int)entities.size() - 1; j >= 0; j--)
        {
            Entity* currentEntity = entities[j]; //cercam entitats
            Vector3 coll;
            Vector3 collnorm;
            //comprovam si colisiona la entitat amb la bala
            if (currentEntity->mesh->testSphereCollision(currentEntity->model, bullet_center, 0.1, coll, collnorm)) {
                //printf("COLLISION BULLET WITH ENTITY\n");
                currentBullet->isActive = false;//si la bala col·lisiona amb una entitat estatica (parets..), elimina la bala
                continue;
            }
        }
        for (int j = (int)enemies.size() - 1; j >= 0; j--)
        {
            Entity* currentEnemy = enemies[j]; //cercam enemics
            Player* currentEnemyPlayer = player_enemies[j];

            if (currentEnemy->current_entity == Entity::ENTITY_ID::ENEMY) { //no faria falta el if, pero es per assegurar que es tracta de un enemic
                Vector3 coll;
                Vector3 collnorm;

                Matrix44 current_model;
                //en cas de ser un enemic, agafem la seva posicio i cap propietat més de la model
                current_model.setTranslation(currentEnemy->model.getTranslation().x, currentEnemy->model.getTranslation().y, currentEnemy->model.getTranslation().z);

                //comprovam si colisiona el enemic amb la bala
                  //g->box_col = MESH de un CUB RECTANGULAR amb les mides d'un enemic, molt millor per comprovar colisions!
                if (g->box_col->testSphereCollision(current_model, bullet_center, 0.2, coll, collnorm ) && currentBullet->author != currentEnemyPlayer->id) {
                Vector3 pos; Vector3 normal;
                //if (currentEnemy->mesh->testRayCollision(currentEnemy->model, bullet_center, entity->dir, pos, normal, 0.5f)) {
                    //printf("COLLISION BULLET WITH ENEMY\n");
                    g->PlayGameSound(g->ChHit_enemy);
                    enemies.erase(enemies.begin() + j);//si l'esfera col·lisiona, elimina a la enitat enemic
                    player_enemies.erase(player_enemies.begin() + j);//si l'esfera col·lisiona, elimina al player enemic
                    currentBullet->isActive = false;//si la bala col·lisiona, elimina la bala
                    continue;
                }
            }
        }
        //Check colisions BALA vs JUGADOR
        Vector3 coll;
        Vector3 collnorm;
        if (g->box_col->testSphereCollision(entityPlayer->model, bullet_center, 0.1, coll, collnorm ) && currentBullet->author != player.id) {
            printf("YOU DIE!\n");
            g->PlayGameSound(g->ChHit_Player);
            currentBullet->isActive = false;//si la bala col·lisiona, elimina la bala
            //restartWorld(levelsWorld, levelsEnemies, currentLevel); //reset del level
            continue;
        }


        
    }
}

Vector3 World::Lerp(Vector3 a, Vector3 b, float t) {
    Vector3 ab = b - a;
    return a + (ab * t);
}

void World::creteGrid() {
    //pathfinding
    map_grid = new uint8[W*H];
    for (size_t i = 0; i < W*H; i++) {
        map_grid[i] = 1;
    }
}
void World::renderPath(bool cameraLocked) {
    Game* g = Game::instance;
    //pathfinding
    if (path_steps > 0) {
        Mesh m;
        for (size_t i = 0; i < path_steps; i++) {
            int index = output[i];
            int x = index % W;
            int y = index / W;
            Vector3 pos;
            pos.x = x * tileSizeX;
            pos.z = y * tileSizeY;
            m.vertices.push_back(pos);
        }
        Entity pathfinding = Entity(Matrix44(), &m, g->texture_black);
        pathfinding.RenderEntity(GL_LINE_STRIP, g->shader, g->camera, cameraLocked);
    }
}
float World::sign(float value) {
    return value >= 0.0f ? 1.0f : -1.0f;
}
void World::restartWorld(std::vector<char*> levelsWorld, std::vector<char*> levelsEnemies, int currentLevel) {
    Game* g = Game::instance; 

    char* pathW = levelsWorld[currentLevel];
    char* pathE = levelsEnemies[currentLevel];

    enemies.clear();
    player_enemies.clear();
    entities.clear();
    
    loadWorld(pathW);
    loadEnemies(pathE);
    player.setSpawnPoint(currentLevel);
}

int World::GetFreeBullet() {
    for (size_t i = 0; i < numBullets; i++)
    {
        Bullet* currentBullet = Bullets[i];
        if (!currentBullet->isActive) {
            return i;
        }
    }
    return -1;
}
void World::RenderBullets(Camera* cam, Shader* shader, bool cameraLocked) {
    for (size_t i = 0; i < numBullets; i++) { //Renderitza totes les bales que es creen
        Bullet*& currentBullet = Bullets[i];

        if (currentBullet->isActive == false) {
            continue;
        }
        Entity* entity = currentBullet->entity_bullet;
        currentBullet->entity_bullet->RenderEntity(GL_TRIANGLES, shader, cam, cameraLocked); //NOTA AQUI NO SA ESTA PINTANT

    }
}

void World::InitBullets(Mesh* mesh, Texture* texture) {

    for (size_t i = 0; i < numBullets; i++)
    {
        Bullets.push_back(new Bullet(Matrix44(), mesh, texture));
    }
}

bool World::checkEnemies() {
    if (enemies.size() == 0) {
        return true;
    }
    return false;
}
