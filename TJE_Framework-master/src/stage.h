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
#include "world.h"
#include "extra/bass.h"	
#include "bullet.h"

enum STAGE_ID {
    INTRO = 0,
    LEVEL = 1,
    NEXTLEVEL = 2,
    FINAL = 3,
    EDITMODE = 4,
	MENU = 5
};


class Stage {
public:

    // NOTA: totes les variables que s'han d'emprar a DIFERENTS Stages derivades, s'han de declara Static... (com World)
	Entity* player_entity = NULL;
	static World world;
    static std::vector<char*> levelsWorld;
    static std::vector<char*> levelsEnemies;
    static int currentLevel;


	Player* player = &world.player;
	int entityToAdd = Entity::ENTITY_ID::BARREL;
	float angle;
	Matrix44 playerModel;

	//bool pause;
	bool wasLeftMousePressed;
	bool slowMotion;

	//Stage(); //crec que no hauria de ser necessaria
    virtual STAGE_ID GetId() = 0;
    virtual void Render(bool cameraLocked) = 0;
    virtual void Update(float seconds_elapsed, bool &cameraLocked) = 0;
    virtual void onKeyDown(SDL_KeyboardEvent event) = 0;
};

class Intro : public Stage {
public:
    STAGE_ID GetId() {return STAGE_ID::INTRO; };
    Intro(); //Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
    void onKeyDown(SDL_KeyboardEvent event);
};

class Level : public Stage {
public:
	//Variables
    static Level*  instance;
	
    STAGE_ID GetId() {return STAGE_ID::LEVEL; };
    Level();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
	void renderSkyGround(Camera* camera, bool cameraLocked);
    void onKeyDown(SDL_KeyboardEvent event);
};

class NextLevel : public Stage {
public:
    static NextLevel* instance;

    STAGE_ID GetId() {return STAGE_ID::NEXTLEVEL; };
    NextLevel();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
    void onKeyDown(SDL_KeyboardEvent event);
};

class Final : public Stage {
public:
    STAGE_ID GetId() {return STAGE_ID::FINAL; };
    Final();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
    void onKeyDown(SDL_KeyboardEvent event);
};

class EditMode : public Stage {
public:
	//Variables 
    Entity* selectedEntity = NULL;

	//funcions
	STAGE_ID GetId() { return STAGE_ID::EDITMODE; };
    EditMode();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
    void onKeyDown(SDL_KeyboardEvent event);
};

class Menu : public Stage {
public:

	//variables
	Entity* selectedEntity = NULL;


	STAGE_ID GetId() { return STAGE_ID::MENU; };
	Menu();//Constructor
	void Render(bool cameraLocked);
	void Update(float seconds_elapsed, bool &cameraLocked);
	void RenderGUI(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV);
	bool RenderButton(float x, float y, float w, float h, Texture* texture, Vector4 color = Vector4(1, 1, 1, 1), bool flipYV = true);
    void onKeyDown(SDL_KeyboardEvent event);

};


#endif /* stage_h */
