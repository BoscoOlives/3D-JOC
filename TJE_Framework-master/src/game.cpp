#include "game.h"
#include "utils.h"
#include "input.h"

#include <cmath>

//#define EDITOR

//bool wasLeftMousePressed = false;

Game* Game::instance = NULL;



Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
    //pause = false;//FET

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;


	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
    
	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,50.f, 50.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective
    
	preRender(); //preRender, PANTALLA DE CARGA

	//Load Mesh i Textures
	loadTexturesMeshesAnimationsShaders();
	
	//inicialitzar part d'audio
	initAudio();
	LoadAllSamples(); //carregam tots els audios


	InitStages();
	previousStage = STAGE_ID::INTRO;
	currentStage = STAGE_ID::INTRO;
	GetCurrent()->world.InitBullets(mesh_bullet, texture_bullet);

	ChIntroMusic = PlayGameSound(introMusic, true);//llançem la musica del joc


	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

   
	camera->enable();

	if (currentStage == STAGE_ID::EDITMODE || currentStage == STAGE_ID::NEXTLEVEL || currentStage == STAGE_ID::YOUDIED) {
		GetStage(STAGE_ID::LEVEL)->Render(cameraLocked);
	}

	GetCurrent()->Render(cameraLocked);

	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	GetCurrent()->Update(seconds_elapsed, cameraLocked);
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	GetCurrent()->onKeyDown(event);

}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}
 
void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT)
	{
		if (currentStage == STAGE_ID::LEVEL) {

			
			if (!GetCurrent()->player->shot && cameraLocked) { //solament pot disparar quan ha acabat la animaci— de disparar				
				GetCurrent()->player->Shoot(GL_TRIANGLES, camera, shader, cameraLocked, GetCurrent()->playerModel,GetCurrent()->player);
				GetCurrent()->player->shot = true;
			}
		}

		if (currentStage == STAGE_ID::MENU || currentStage == STAGE_ID::INTRO || currentStage == STAGE_ID::CONTROLS || currentStage == STAGE_ID::YOUDIED || currentStage == STAGE_ID::FINAL) {
			GetCurrent()->wasLeftMousePressed = true;
		}
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::loadTexturesMeshesAnimationsShaders() {
	//BOTONS
	texture_black = texture_black->getBlackTexture();
	texture_white = texture_black->getWhiteTexture();

	playMenu = Texture::Get("data/gui/playMenu.png");
	restartMenu = Texture::Get("data/gui/restartMenu.png");
	restartBlack = Texture::Get("data/gui/restart.png");
	saveMenu = Texture::Get("data/gui/saveMenu.png");
    exitMenu = Texture::Get("data/gui/exitMenu.png");
    ctrlsMenu = Texture::Get("data/gui/ctrlsMenu.png");
	menuBorder = Texture::Get("data/gui/menuBorder.png");

    
	exit = Texture::Get("data/gui/exit_.png");
	controls = Texture::Get("data/gui/controls.png");
	back = Texture::Get("data/gui/back.png");
	load = Texture::Get("data/gui/load.png");
	newGame = Texture::Get("data/gui/newGame.png");
	ctrls = Texture::Get("data/gui/ctrls.png");
	title = Texture::Get("data/gui/title.png");
	
    
	nexetLevel = Texture::Get("data/gui/nextlevel.png");
    titleBackground = Texture::Get("data/gui/titleBackground.png");
	youDied = Texture::Get("data/gui/youdied.png");
	finalScreen = Texture::Get("data/gui/finalScreen.png");



	//mesh_sphere = Mesh::Get("data/sphere.obj");

	mesh_ground = new Mesh();
	mesh_ground->createPlane(30);
	texture_ground = Texture::Get("data/levels/ground.png");
	
	mesh_pistol = Mesh::Get("data/levels/obj_player/pistol.obj");

	mesh_pistol_e = Mesh::Get("data/levels/obj_player/pistol_enemy.obj");
	texture_pistol  = Texture::Get("data/levels/obj_player/color-atlas-new.png");

	texture_sky = Texture::Get("data/sky/spaceclouds.png");
	mesh_sky = Mesh::Get("data/sky/sky.ASE");

	
	mesh_bullet = Mesh::Get("data/levels/obj_player/bullet.obj");
	texture_bullet = Texture::Get("data/levels/obj_player/bullet.png");

	//meshes per carregar in game
	mesh_rock1 = Mesh::Get("data/levels/obj_scene/rocksTallOre.obj");
	texture_rock1 = Texture::Get("data/levels/obj_scene/rocksTallOre.png");
	mesh_barrel = Mesh::Get("data/levels/obj_scene/barrel.obj");
	mesh_consoleScreen = Mesh::Get("data/levels/obj_scene/consoleScreen.obj");
	mesh_SupportCorner = Mesh::Get("data/levels/obj_scene/monorailSupportCorner_exclusive.obj");
	texture_barrel = Texture::Get("data/levels/obj_scene/barrel.png");
	texture_consoleScreen = Texture::Get("data/levels/obj_scene/consoleScreen.png");
	texture_SupportCorner = Texture::Get("data/levels/obj_scene/monorailSupportCorner_exclusive.png");

	//imports d'arxius d'animació
	mesh_cowboy_run = Mesh::Get("data/animation/cowboy_run.mesh");
	mesh_cowboy_idle = Mesh::Get("data/animation/cowboy_idle.mesh");
	texture_cowboy = Texture::Get("data/animation/westernpack.png");
	anim_run = Animation::Get("data/animation/cowboy_run.skanim");
	anim_idle = Animation::Get("data/animation/cowboy_idle.skanim");
	box_col = Mesh::Get("data/levels/obj_player/box_colision_enemy.obj");

	//we load a shader
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	//descomentar si es volen Shaders  sense iluminacio
	//shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	light = new Light();
	light->calcKaia();

	anim_shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/phong.fs");


	
}

void Game::initAudio(){
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		std::cout << "ERROR initializing audio" << std::endl;
	}
}
HSAMPLE Game::LoadSample(const char* fileName) {
	//El handler para un sample
	HSAMPLE hSample;

	//Cargamos un sample del disco duro (memoria, filename, offset, length, max, flags)
	//use BASS_SAMPLE_LOOP in the last param to have a looped sound
	hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, 0);
	if (hSample == 0)
	{
		std::cout << "ERROR load" << fileName << std::endl;
	}
	std::cout << " + AUDIO load" << fileName << std::endl;
	return hSample;
}
HCHANNEL Game::PlayGameSound(HSAMPLE fileSample, bool LOOP, float vol) {
	HCHANNEL fileChannel = BASS_SampleGetChannel(fileSample, false);
	//si esteim en SlowMotion, baixam la freq. de mostreig de canal (original = 44100Hz)
	if(GetCurrent()->slowMotion){ BASS_ChannelSetAttribute(fileChannel, BASS_ATTRIB_FREQ, 15000); }
	else { BASS_ChannelSetAttribute(fileChannel, BASS_ATTRIB_FREQ, 0); }
	
	BASS_ChannelSetAttribute(fileChannel, BASS_ATTRIB_VOL, vol); //volumen del dispar (el podriem controlar amb una variable des del menu!
	
	if(LOOP){ BASS_ChannelFlags(fileChannel, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP); ; }
	//Lanzamos un sample
	BASS_ChannelPlay(fileChannel, true);
	return fileChannel;

}

void Game::StopGameSound(HCHANNEL fileSample) {

	//Lanzamos un stop
	BASS_ChannelStop(fileSample);

}
void Game::LoadAllSamples() {
	shoot = LoadSample("data/audios/paintball.wav");
	recoil = LoadSample("data/audios/recoil.wav");
	hit_enemy = LoadSample("data/audios/hit_enemy.wav");
	hit_player = LoadSample("data/audios/hit_player.wav");
	boton = LoadSample("data/audios/boton.wav");
	AudioExit = LoadSample("data/audios/exit.wav");
	introMusic = LoadSample("data/audios/Elevxte.mp3");
	ambient = LoadSample("data/audios/ambient_in_game.mp3");

}

Stage* Game::GetStage(STAGE_ID id) {
	return stages[(int)id];
}
Stage* Game::GetCurrent() {
	return GetStage(currentStage);
}
void Game::SetStage(STAGE_ID id) {
	previousStage = currentStage;
	currentStage = id;
}


void Game::InitStages() {
	stages.reserve(8);
	stages.push_back(new Intro());
	stages.push_back(new Level());
	stages.push_back(new NextLevel());
	stages.push_back(new Final());
	stages.push_back(new EditMode());
	stages.push_back(new Menu());
	stages.push_back(new Controls());
	stages.push_back(new YouDied());
}

void Game::preRender(void) {
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	camera->enable();

	Texture* loadScreen = Texture::Get("data/gui/LoadScreen.png");
	Mesh quad;
	quad.createQuad(window_width / 2, window_height / 2, window_width, window_height, true);

	Camera cam2D;
	cam2D.setOrthographic(0, window_width, window_height, 0, -1, 1);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	if (!shader) return;
	shader->enable();
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	if (loadScreen != NULL) {
		shader->setUniform("u_texture", loadScreen, 0);
	}
	shader->setUniform("u_time", time);
	//shader->setUniform("u_tex_tiling", 1.0f);
	shader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);
	shader->disable();

	SDL_GL_SwapWindow(this->window);
}