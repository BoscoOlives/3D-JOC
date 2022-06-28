//
//  world.h
//  TJE_XCODE
//
//  Created by Josep Ricci on 15/5/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#ifndef world_h
#define world_h

#include "utils.h"
#include "includes.h"
#include "player.h"
#include "entity.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "input.h"
#include "pathfinders.h"
#include "bullet.h"



class World {
public:
    static World* instance;
    Player player = Player(0); //ID jugador es 0

    //buildings, stages, scores...
    std::vector<Entity*> entities;
    std::vector<Entity*> enemies;
    std::vector<Bullet*> Bullets;
    std::vector<Player*> player_enemies;
    //pathfinding
    int start_x;
    int start_y;
    int target_x;
    int target_y;
    uint8* map_grid;
    int output[100];
    int W = 100;
    int H = 100;
    float tileSizeX = 1.0f;
    float tileSizeY = 1.0f;
    int path_steps;
    const int numBullets = 50;
    

    //Bullet* Bullets[numBullets];

    

    World();

    void saveEnemies(char* path);
    void loadEnemies(char* path);
    void loadWorld(char* path);

    void AddEntityInFront(Camera* cam, Entity::ENTITY_ID entityToAdd);
    Entity* RayPick(Camera* cam, Entity* selectedEntity, float max_ray_dist = 1e+10F);
    void RotateSelected(float angleDegrees, Entity* selectedEntity);
    void DeleteEntity(Camera* cam);
    void get_Mesh_Texture_Entity(int id, Mesh*& mesh, Texture*& texture);
    void shooting_update(Entity*& entityPlayer, std::vector<char*> levelsWorld, std::vector<char*> levelsEnemies, int currentLevel);
    Vector3 Lerp(Vector3 a, Vector3 b, float t);
    void creteGrid();
    void renderPath(bool cameraLocked);
    float sign(float value);
    void restartWorld(std::vector<char*> levelsWorld, std::vector<char*> levelsEnemies, int currentLevel);
    int GetFreeBullet();
    void RenderBullets(Camera* cam, Shader* shader, bool cameraLocked);
    void InitBullets(Mesh* mesh, Texture* texture);
    bool checkEnemies();

};

#endif /* world_h */
