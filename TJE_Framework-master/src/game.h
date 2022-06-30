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
//#include <bass.h>
#include "extra/bass.h"	
#include "stage.h"
#include "bullet.h"
#include "light.h"


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
	bool cameraLocked;
	//Variables	BOSCO &	RICCI
	/*std::vector<Entity*> entities;
	std::vector<Entity*> enemies;
	std::vector<Entity*> bullets;
	std::vector<Vector3> points;*/
	//std::vector<Player*> player_enemies;
    Entity* selectedEntity = NULL;
	//Entity* player_entity = NULL;
	//World world;
	//Player* player = &world.player;
	Texture* texture_black = NULL;
	Texture* texture_white = NULL;
	//bool slowMotion;
	//bool cameraLocked;
	float mouse_speed;
	Mesh* mesh_sky = NULL;
	Mesh* mesh_pistol = NULL;
	Mesh* mesh_pistol_e = NULL;
	Mesh* mesh_rock1 = NULL;
	Mesh* mesh_bullet = NULL;
	Mesh* mesh_cowboy_idle = NULL;
	Mesh* mesh_cowboy_run = NULL;
	Mesh* box_col = NULL;
	Mesh* mesh_sphere;
	Texture* texture_cowboy = NULL;
	Texture* texture_bullet = NULL;
	Texture* texture_sky = NULL;
	Texture* texture_rock1 = NULL;
	Texture* texture_pistol = NULL;

	Mesh* mesh_ground = NULL;
	Texture* texture_ground = NULL;
	Shader* shader = NULL;
	Shader* anim_shader = NULL;
	Animation* anim = NULL;
	FBO* fbo = NULL;
	//Matrix44 playerModel;
	Animation* anim_idle;
	Animation* anim_run;
	
	//iluminacio
	Shader* phong_shader = NULL;
	Light* light = NULL;

	//meshes per carregar in game
	Mesh* mesh_barrel = NULL;
	Mesh* mesh_consoleScreen = NULL;
	Mesh* mesh_SupportCorner = NULL;
	Texture* texture_barrel = NULL;
	Texture* texture_consoleScreen = NULL;
	Texture* texture_SupportCorner = NULL;


	//botons
	//Texture* play;
	//Texture* restart;
	//Texture* save;
	Texture* exit;
	Texture* volumeOn;
	Texture* volumeOff;
	Texture* controls;
	Texture* back;
	Texture* load;
	Texture* newGame;
	Texture* ctrls;
    
    Texture* ctrlsMenu;
    Texture* exitMenu;
    Texture* restartMenu;
    Texture* playMenu;
    Texture* saveMenu;
    Texture* menuBorder;

	Texture* nexetLevel;
    
    Texture* titleBackground;

	//Audios and Channels
	HSAMPLE shoot;
	HCHANNEL ChShoot;
	HSAMPLE recoil;
	HCHANNEL ChRecoil;
	HSAMPLE hit_enemy;
	HCHANNEL ChHit_enemy;
	HSAMPLE hit_player;
	HCHANNEL ChHit_Player;
	HSAMPLE boton;
	HCHANNEL ChBoton;
	HSAMPLE AudioExit;
	HCHANNEL ChAudioExit;
	HSAMPLE introMusic;
	HCHANNEL ChIntroMusic;

	std::vector<Stage*> stages;
	STAGE_ID currentStage;
	STAGE_ID previousStage;
    

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
	void initAudio();
	HSAMPLE LoadSample(const char* fileName);
	void PlayGameSound(HCHANNEL fileSample, bool LOOP = false);
	void StopGameSound(HCHANNEL fileSample);
	void LoadAllSamples();
	//void RenderGUI(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV);
	//bool RenderButton(float x, float y, float w, float h, Texture* texture, Vector4 color = Vector4(1, 1, 1, 1), bool flipYV = true);
	
	//funcions Stages
	void InitStages();
	Stage* GetStage(STAGE_ID id);
	Stage* GetCurrent();
	void SetStage(STAGE_ID id);
};


#endif 
