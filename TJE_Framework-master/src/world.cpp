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
    //this->player = Player();
}

void World::saveWorld(std::vector<Entity*> entities) {
    //save player, enemies (positions, rotations...)
    printf("Saving World...\n");
    FILE* file = fopen("world.txt", "wb");
    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* entity = entities[i];
        Matrix44 model = entity->model;

        //for (size_t j = 0; j < 15; j++)
        //{
        //    std::string str = { std::to_string(model.m[j]) + "\n" };
        //    const char* buffer = str.c_str();
        //    fwrite(buffer, strlen(buffer) / 4 + 1, sizeof(buffer), file);
        //}

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
std::vector<Entity*> World::loadWorld(std::vector<Entity*> entities) {
    //load player, enemies (positions, rotations...)
    Game* g = Game::instance;
    Mesh* mesh;
    Texture* texture;
    int id;
    std::string line;
    std::ifstream myfile("world.txt");

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
                
                entities.push_back(entity);
                i = -1;
            }
            std::cout << line << "\n";
            i = i+1;
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";
    return entities;

    printf("\nLoading World...\n");

}

void World::get_Mesh_Texture_Entity(int id, Mesh* &mesh, Texture* &texture) {//funció per obtenir la mesh i la texture depenent de la ID
    Game* g = Game::instance;

    if (id == Entity::ENTITY_ID::HOUSE) {
        mesh = g->mesh_house;
        texture = g->texture_black;
    }
    else if (id == Entity::ENTITY_ID::WALL) {
        mesh = g->mesh_wall;
        texture = g->texture_wall;
    }
    else if (id == Entity::ENTITY_ID::ENEMY) {
        mesh = g->mesh_cowboy_run;
        texture = g->texture_cowboy;
    }
    else if (id == Entity::ENTITY_ID::RING) {
        mesh = g->mesh_ring;
        texture = g->texture_ring;
    }
    else if (id == Entity::ENTITY_ID::ZONA0) {
        mesh = g->mesh_zona0;
        texture = g->texture_zona0;
    }
    else if (id == Entity::ENTITY_ID::ZONA1) {
        mesh = g->mesh_zona1;
        texture = g->texture_zona1;
    }
}

std::vector<Entity*> World::AddEntityInFront(Camera* cam, int entityToAdd, std::vector<Entity*> entities) {
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
    if (entityToAdd == 6){
        model.scale(0.01, 0.01, 0.01);
        Player* player = new Player();
        player->pos = spawnPos;
        g->player_enemies.push_back(player);
        
    }

    Entity* entity = new Entity(model, mesh, texture);
    entity->current_entity = (Entity::ENTITY_ID)entityToAdd;

    entities.push_back(entity);
    return entities;
}

Entity* World::RayPick(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities, Entity* selectedEntity, float max_ray_dist) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* entity = entities[i];
        Vector3 pos;
        Vector3 normal;
        //testRayCollision(Matrix44 model, Vector3 start, Vector3 front, Vector3& collision, Vector3& normal, float max_ray_dist, bool in_object_space )
        if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal, max_ray_dist)) {
            //std::cout << "Selected" << std::endl;
            //points.push_back(pos);
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

std::vector<Entity*> World::DeleteEntity(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities) {
    
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
            return entities;
        }
    }
    printf("No Entity Removed\n");
    return entities;
     
}

void World::shooting_update(std::vector<Entity*> &entities, std::vector<Entity*> &enemies, std::vector<Entity*>& bullets) {
    // FOR LOOP PER FER UPDATE DE LA POSICIÓ DE LA BALA
    Game* g = Game::instance;
    for (int i = (int)bullets.size()-1; i >= 0; i--) //bullets.size
    {
        Entity* entity = bullets[i]; // cercam les BULLETS
        float vel = 5.0f;
        entity->update_position_moving(g->elapsed_time, vel);
            
        Vector3 bullet_center = entity->model.getTranslation();
        if (bullet_center.y < 0.0f) { //si la bala atravessa el terra, elimina la bala
            bullets.erase(bullets.begin() + i);
            continue;
        }
        Vector3 delete_dist = bullet_center - g->player->pos;
        if (delete_dist.length() > 20.0f) { //si la bala es massa lluny, elimina la bala
            bullets.erase(bullets.begin() + i);
            continue;
        }

        for (int j = (int)entities.size() - 1; j >= 0; j--)
        {
            Entity* currentEntity = entities[j]; //cercam entitats
            Vector3 coll;
            Vector3 collnorm;
            //comprovam si colisiona  la entitat amb la bala
            if (currentEntity->mesh->testSphereCollision(currentEntity->model, bullet_center, 0.1, coll, collnorm)) {
                printf("COLLISION BULLET WITH ENTITY\n");
                bullets.erase(bullets.begin() + i);//si la bala col·lisiona, elimina la bala
                continue;
            }
        }
        for (int j = (int)enemies.size() - 1; j >= 0; j--)
        {
            Entity* currentEnemy = enemies[j]; //cercam enemics

            if (currentEnemy->current_entity == Entity::ENTITY_ID::ENEMY) { //no faria falta el if, pero es per assegurar que es tracta de un enemic
                Vector3 coll;
                Vector3 collnorm;
                //comprovam si colisiona el enemic amb la bala
                currentEnemy->model.scale(2.0, 2.0, 2.0);

                if (currentEnemy->mesh->testSphereCollision(currentEnemy->model, bullet_center, 0.2, coll, collnorm)) { //NOTA: la colisio esta en els peus, hauriem de pensar algo
                Vector3 pos; Vector3 normal;
                //currentEnemy->mesh->testRayCollision(<#Matrix44 model#>, <#Vector3 ray_origin#>, <#Vector3 ray_direction#>, <#Vector3 &collision#>, <#Vector3 &normal#>)
                //if (currentEnemy->mesh->testRayCollision(currentEnemy->model, bullet_center, entity->dir, pos, normal, 0.5f)) {
                    printf("COLLISION BULLET WITH ENEMY\n");
                    enemies.erase(enemies.begin() + j);//si l'esfera col·lisiona, elimina a la enitat enemic
                    g->player_enemies.erase(g->player_enemies.begin() + j);//si l'esfera col·lisiona, elimina al player enemic
                    bullets.erase(bullets.begin() + i);//si la bala col·lisiona, elimina la bala
                    continue;
                }
            }
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
void World::renderPath() {
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
        pathfinding.RenderEntity(GL_LINE_STRIP, g->shader, g->camera, g->cameraLocked);
    }
}
float World::sign(float value) {
    return value >= 0.0f ? 1.0f : -1.0f;
}

