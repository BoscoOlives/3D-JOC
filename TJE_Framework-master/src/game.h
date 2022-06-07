/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "entity.h"
#include "world.h"
#include "player.h"
#include "animation.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include <bass.h>

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	//Variables	BOSCO &	RICCI
	std::vector<Entity*> entities;
	std::vector<Entity*> enemies;
	std::vector<Vector3> points;
    Entity* selectedEntity = NULL;
	World world;
	Player* player = &world.player;
	std::vector<Player*> player_enemies;
	Texture* texture_black = NULL;
	Texture* texture_white = NULL;
	bool slowMotion;
	bool cameraLocked;
	int entityToAdd = Entity::ENTITY_ID::HOUSE;
	float angle;
	float mouse_speed;
	Mesh* mesh_sky = NULL;
	Mesh* mesh_house = NULL;
	Mesh* mesh_wall = NULL;
	Mesh* mesh_man = NULL;
	Mesh* mesh_pistol = NULL;
	Mesh* mesh_pistol_e = NULL;
	Mesh* mesh_ring = NULL;
	Mesh* mesh_zona0 = NULL;
	Mesh* mesh_zona1 = NULL;
	Mesh* mesh_bullet = NULL;
	Mesh* mesh_cowboy_walk = NULL;
	Mesh* mesh_cowboy_run = NULL;
	Texture* texture_cowboy = NULL;
	Texture* texture_bullet = NULL;
	Texture* texture_zona0 = NULL;
	Texture* texture_zona1 = NULL;
	Texture* texture_sky = NULL;
	Texture* texture_ring = NULL;
	Texture* texture_wall = NULL;
	Mesh* mesh_ground = NULL;
	Texture* texture_ground = NULL;
	Shader* shader = NULL;
	Shader* anim_shader = NULL;
	Animation* anim = NULL;
	FBO* fbo = NULL;
	Matrix44 playerModel;
	//Vector3 character_center;
	Animation* anim_walk;
	Animation* anim_run;
	HSAMPLE shoot;
	
    

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	//funcions BOSCO & RICCI
	void loadTexturesAndMeshes();
	HSAMPLE LoadSample(const char* fileName);
	void PlayGameSound(HSAMPLE fileSample);
};


#endif 
