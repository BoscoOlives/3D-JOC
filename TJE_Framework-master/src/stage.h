//
//  stage.h
//  TJE_XCODE
//
//  Created by Josep Ricci on 10/6/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#ifndef stage_h
#define stage_h

#include "utils.h"
#include "includes.h"
#include "player.h"
#include "entity.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "input.h"
#include "pathfinders.h"

enum StageID {
    INTRO = 0,
    TUTORIAL = 1,
    LEVEL = 2,
    FINAL = 3
};

class Stage {
public:
    virtual StageID GetId() = 0;
    virtual void Render(Image& framebuffer) = 0;
    virtual void Update(float elapsed_time) = 0;
};
std::vector<Stage*> stages;

class Intro : public Stage {
public:
    StageID GetId() {return StageID::INTRO; };

    void render();
    void update();
};

class Tutorial : public Stage {
public:
    StageID GetId() {return StageID::TUTORIAL; };

    void render();
    void update();
};

class Level : public Stage {
public:
    StageID GetId() {return StageID::LEVEL; };

    void render();
    void update();
};

class Final : public Stage {
public:
    StageID GetId() {return StageID::FINAL; };

    void render();
    void update();
};



#endif /* stage_h */
