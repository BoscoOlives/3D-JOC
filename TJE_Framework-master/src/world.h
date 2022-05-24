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

class World {
public:
    static World* instance;
    Player player;
    std::vector<Player*> enemies;
    std::vector<Entity*> entities;
    //buildings, stages, scores...

    World();

    void saveWorld(std::vector<Entity*> entities);
    std::vector<Entity*> loadWorld(std::vector<Entity*> entities);

    std::vector<Entity*> AddEntityInFront(Camera* cam, int entityToAdd, std::vector<Entity*> entities);
    Entity* RayPick(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities, Entity* selectedEntity);
    void RotateSelected(float angleDegrees, Entity* selectedEntity);
    std::vector<Entity*> DeleteEntity(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities);
    void get_Mesh_Texture_Entity(int id, Mesh*& mesh, Texture*& texture);
    std::vector<Entity*> shooting_update(std::vector<Entity*> entities);
    Vector3 Lerp(Vector3 a, Vector3 b, float t);
};

#endif /* world_h */
