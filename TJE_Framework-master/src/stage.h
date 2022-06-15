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


enum STAGE_ID {
    INTRO = 0,
    TUTORIAL = 1,
    LEVEL = 2,
    FINAL = 3,
    EDITMODE = 4,
	MENU = 5
};


class Stage {
public:
	/*std::vector<Entity*>* entities = &world.entities;
	std::vector<Entity*>* enemies = &world.enemies;
	std::vector<Entity*>* bullets = &world.bullets;
	std::vector<Vector3>* points = &world.points;
	std::vector<Player*>* player_enemies = &world.player_enemies;*/
	Entity* player_entity = NULL;
	World world;
	Player* player = &world.player;
	
	int entityToAdd = Entity::ENTITY_ID::HOUSE;
	float angle;
	Matrix44 playerModel;

	float load_distance = 200.0f;
	float no_render_distance = 1000.0f;
	bool pause;
	bool wasLeftMousePressed;
	bool slowMotion;

	//Stage(); //crec que no hauria de ser necessaria
    virtual STAGE_ID GetId() = 0;
    virtual void Render(bool cameraLocked) = 0;
    virtual void Update(float seconds_elapsed, bool &cameraLocked) = 0;
};
//std::vector<Stage*> stages;

class Intro : public Stage {
public:
    STAGE_ID GetId() {return STAGE_ID::INTRO; };
    Intro(); //Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
};

class Tutorial : public Stage {
public:
	//Variables
    static Tutorial*  instance;
	

    STAGE_ID GetId() {return STAGE_ID::TUTORIAL; };
    Tutorial();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
	void renderSkyGround(Camera* camera, bool cameraLocked);

};

class Level : public Stage {
public:
    static Tutorial* instance;

    STAGE_ID GetId() {return STAGE_ID::LEVEL; };
    Level();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
};

class Final : public Stage {
public:
    STAGE_ID GetId() {return STAGE_ID::FINAL; };
    Final();//Constructor
    void Render(bool cameraLocked);
    void Update(float seconds_elapsed, bool &cameraLocked);
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

};


#endif /* stage_h */
