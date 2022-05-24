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
	std::vector<Vector3> points;
    Entity* selectedEntity = NULL;
	World world;
	Player* player = &world.player;
	Texture* texture_black = NULL;
	Texture* texture_white = NULL;
    bool slowMotion = false;
	int entityToAdd = Entity::ENTITY_ID::HOUSE;
	Mesh* mesh_house = NULL;
	Mesh* mesh_wall = NULL;
	Mesh* mesh_man = NULL;
	Mesh* mesh_pistol = NULL;
	Mesh* mesh_ring = NULL;
	Texture* texture_ring = NULL;
	Texture* texture_wall = NULL;

	
	
    

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

};


#endif 
