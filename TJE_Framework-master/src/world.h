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
    
    void saveWorld();
    void loadWorld();
    
    void AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture, int window_width, int window_height);
};




#endif /* world_h */
