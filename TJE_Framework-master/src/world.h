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

class World {
public:
    static World* instance;
    Player player = Player(0);
    std::vector<Player*> enemies;
    std::vector<Entity*> entities;
    //buildings, stages, scores...

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

    World();

    void saveWorld(std::vector<Entity*> entities);
    std::vector<Entity*> loadWorld(std::vector<Entity*> entities);

    std::vector<Entity*> AddEntityInFront(Camera* cam, int entityToAdd, std::vector<Entity*> entities);
    Entity* RayPick(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities, Entity* selectedEntity, float max_ray_dist = 1e+10F);
    void RotateSelected(float angleDegrees, Entity* selectedEntity);
    std::vector<Entity*> DeleteEntity(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities);
    void get_Mesh_Texture_Entity(int id, Mesh*& mesh, Texture*& texture);
    void shooting_update(std::vector<Entity*> &entities, std::vector<Entity*> &enemies, std::vector<Entity*>& bullets);
    Vector3 Lerp(Vector3 a, Vector3 b, float t);
    void creteGrid();
    void renderPath();
    float sign(float value);
};

#endif /* world_h */
