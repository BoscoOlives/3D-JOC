//
//  world.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "world.h"
#include "Game.h"

World* World::instance = NULL;

class Prop { //SERVIRA PER EXPORTAR EN UN .TXT TOTA LA INFO DE ON GENEREM LES ENTITATS I UN IDENTIFICADOR (video Programando un Mundo 1:50:00)
    int id;
    Mesh* mesh;
    Texture* texture;
};
Prop props[20];
/*Funció que guarda el món:
ID Entity int
POSITION Vector3
ROTATION Vector3
SCALE Vector3


escupir Model
path Mesh
path Texture

*/

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
        std::string str = {
            std::to_string(model._11)+"\n"+std::to_string(model._12)+"\n"+std::to_string(model._13)+"\n"+std::to_string(model._14)+"\n"+
            std::to_string(model._21)+"\n"+ std::to_string(model._22)+"\n"+ std::to_string(model._23)+"\n"+std::to_string(model._24)+"\n"+
            std::to_string(model._31)+"\n"+ std::to_string(model._32)+"\n"+ std::to_string(model._33)+"\n"+ std::to_string(model._34)+"\n"+
            std::to_string(model._41) + "\n" + std::to_string(model._42) + "\n" + std::to_string(model._43) + "\n" + std::to_string(model._44) + "\n" };

        const char* buffer = str.c_str();
        fwrite(buffer, int(strlen(buffer) / 4), sizeof(buffer), file);
    }
    fclose(file);
}
void World::loadWorld() {
    //load player, enemies (positions, rotations...)
    
    FILE* file;
    long lSize;
    char* buffer;
    size_t result;
    file = fopen("world.txt", "rb");

    if (file == NULL) { printf("No exist world file!\n");}
    
    else {
        
        // obtain file size:
        fseek(file, 0, SEEK_END);
        lSize = ftell(file);
        rewind(file);

        // allocate memory to contain the whole file:
        buffer = (char*)malloc(sizeof(char) * lSize); //aixo guarda tots els valors del fitxer seguits, nose com accedir LINE per LINE

        // copy the file into the buffer:
        result = fread(buffer, 1, lSize, file);

        std::cout << buffer << std::endl;

        // terminate

        fclose(file);
        free(buffer);
        printf("Loading World...\n");
    }

}

std::vector<Entity*> World::AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture, std::vector<Entity*> entities) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
    Matrix44 model;
    model.translate(spawnPos.x, spawnPos.y, spawnPos.z);

    Entity* entity = new Entity(model, mesh, texture);
    entities.push_back(entity);
    return entities;
}

Entity* World::RayPick(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities, Entity* selectedEntity) {
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    for (size_t i = 0; i < entities.size(); i++)
    {
        Entity* entity = entities[i];
        Vector3 pos;
        Vector3 normal;
        if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
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

std::vector<Entity*> World::DeleteEntity(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities, Entity* selectedEntity) {
    
    Vector2 mousePos = Input::mouse_position;
    Game* g = Game::instance;
    Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
    Vector3 rayOrigin = cam->eye;

    for (size_t i = 0; i < entities.size(); i++)
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