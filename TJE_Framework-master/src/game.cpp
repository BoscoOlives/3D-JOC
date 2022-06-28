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
	//cameraLocked = false;
	//slowMotion = false;//FET
	//mouse_speed = 100.0f;//FET

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
    
	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,50.f, 50.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective
    
	//Load Mesh i Textures
	loadTexturesAndMeshes();
	
	//inicialitzar part d'audio
	initAudio();
	LoadAllSamples(); //carregam tots els audios
	printf("ALL LOADS OK!\n");
	
	//player->enemy = false; //FET
	////CREAR JUGADOR
	//player_entity = new Entity(playerModel, mesh_pistol, texture_black); //creem la entitat Jugador//FET
	//world.restartWorld();//FET


	InitStages();
	currentStage = STAGE_ID::TUTORIAL;
	GetCurrent()->world.InitBullets(mesh_bullet, texture_bullet);

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
	if (currentStage == STAGE_ID::EDITMODE) {
		GetStage(STAGE_ID::TUTORIAL)->Render(cameraLocked);
	}
	GetCurrent()->Render(cameraLocked);

    
//    
//    Matrix44 skyModel;
//    skyModel.translate(camera->eye.x, camera->eye.y - 40.0f, camera->eye.z);
//    Entity background = Entity(skyModel, mesh_sky, texture_sky);
//    glDisable(GL_DEPTH_TEST); //desactivem el DEPTH TEST abans de redenritzar el fons perque no es superposi devant entitats
//    background.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
//    glEnable(GL_DEPTH_TEST);
//	Matrix44 groundModel;
//    Entity ground = Entity(groundModel, mesh_ground, texture_ground);
//    ground.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
//
//	playerModel.setTranslation(player->pos.x, player->pos.y, player->pos.z);
//
//	playerModel.rotate(player->yaw * DEG2RAD, Vector3(0, 1, 0));
//	playerModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));
//
//
//
//	if (cameraLocked) {
//
//		Matrix44 camModel = playerModel;
//
//		Vector3 eye = playerModel * Vector3(0, 0.7, 0.5); //segon valor altura camera
//		//Vector3 eye = world.Lerp(camera->eye, desiredeEye, 100.f * elapsed_time); //Lerp perque es vegi sa pistola una mica de costat quan ens movem
//		Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
//		Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));
//		
//		camera->lookAt(eye, center, up);
//	}
//	playerModel.translate(0.1, 0, 0);
//	if (player->shot) { //moviment que provoca un shot a la arma
//		playerModel = player->Coil(elapsed_time, playerModel);
//
//	}
//    //CREAR JUGADOR
//	/*Entity player_entity = Entity(playerModel, mesh_pistol, texture_black);*/
//	//render Player
//	player_entity->model = playerModel;
//	player_entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
//
//	//Render de la nostra colisio!
//	/*Entity* box = new Entity(playerModel, box_col, texture_black);
//	box->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);*/
//
//    //render de totes les entitats (estatiques)
//	for (size_t i = 0; i < entities.size(); i++) { //Renderitza totes les entitats que es creen
//		Entity* entity = entities[i];
//		entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
//	}
//
//	//render de tots els enemics
//	for (size_t i = 0; i < enemies.size(); i++) { //Renderitza tots els enemics que es creen
//		Entity* entity = enemies[i];
//		Player* enemy = player_enemies[i];
//		entity->RenderEntityAnim(GL_TRIANGLES, anim_shader, camera, enemy->pos, enemy->yaw, enemy->look);
//
//		//render Colision BOX ENEMY
//		/*Matrix44 box_model;
//		box_model.setTranslation(entity->model.getTranslation().x, entity->model.getTranslation().y, entity->model.getTranslation().z);
//		Entity* box = new Entity(box_model, box_col, texture_black);
//		box->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);*/
//	}
//
//	//render de totes les bales
//	for (size_t i = 0; i < bullets.size(); i++) { //Renderitza totes les bales que es creen
//		Entity* entity = bullets[i];
//		entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
//	}
//    
//	if (!cameraLocked) {//TEXT TECLES MODE EDICIÓ
//		std::string text_edicio = "F1 Reload All\n 0 Save World\n 2 Add Entity\n 3 Select Entity\n 4 Rotate <-\n 5 Rotate ->\n 6 Remove Entity\n 9 Load World\n + Change Entity to Add\n";
//		drawText(this->window_width-200, 2, text_edicio, Vector3(1, 1, 1), 2);
//	}
//	if (cameraLocked) {//TEXT TECLES MODE GAMEPLAY
//		std::string text_gameplay = "LeftMouse Shot\nWASD Move Player\nMouse Move Camera\n ESC Menu\n";
//		drawText(this->window_width - 200, 2, text_gameplay, Vector3(1, 1, 1), 2);
//		drawText(this->window_width/2, this->window_height / 2, "+", Vector3(1, 1, 1), 2);
//		std:string num_enemies = "N. Enemies" + to_string((unsigned int)enemies.size());
//		drawText(2, 20, num_enemies, Vector3(1, 1, 1), 2);
//
//		//printf("%d", (unsigned int)entities.size());
//	}
//
//	//Pathfinding
//	//world.renderPath(cameraLocked);
//
//	//Draw the floor grid
//	drawGrid();
//
//	
//
//    
//    //Render All GUI -----------------------------------
//    if (pause) {
//        glDisable(GL_DEPTH_TEST);
//        glDisable(GL_CULL_FACE);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        //
//
//
//        if (RenderButton(window_width / 2, 100, 600, 100, play)) {
//			cameraLocked = true;
//            pause = false;
//
//            printf("Play\n");
//        }
//        else if (RenderButton(window_width / 2, 200, 600, 100, restart)) {
//			world.restartWorld();
//            printf("Restart\n");
//        }
//        else if (RenderButton(window_width / 2, 300, 600, 100, save)) {
//            printf("Save\n");
//        }
//        else if (RenderButton(window_width/2, 400, 600, 100, exit)) {
//            printf("Exit\n");
//            must_exit = true;
//        }
//    }
//
//    
////    glEnable(GL_DEPTH_TEST);
////    glEnable(GL_CULL_FACE);
////    glDisable(GL_BLEND);
//    
//    //--------------------------------------------------
//    
//    wasLeftMousePressed = false;
//

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	GetCurrent()->Update(seconds_elapsed, cameraLocked);
	//printf("%d", currentStage);
    //if (pause) return; //si pausa, sortim de la funci—
    
	//slowMotion = true;    
	//float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//mouse input to rotate the cam 
	//if (!cameraLocked) {
	//	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed? NO ENTENC PER OR DE MOUSE_LOCKED
	//	{
	//		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
	//		camera->rotate(Input::mouse_delta.y * 0.005f, Vector3(-1.0f,0.0f,0.0f));
	//	}
	//}

	//if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
	//	cameraLocked = !cameraLocked;
	//}
    
	//SDL_ShowCursor(!cameraLocked);
	//if (cameraLocked) { //moviment player
	//	float playerSpeed = 2.5f * elapsed_time;
	//	float rotSpeed = 120.0f * elapsed_time;
	//	
	//	Input::centerMouse();
	//	player->pitch += -Input::mouse_delta.y * 10.0f * elapsed_time;
	//	player->yaw += -Input::mouse_delta.x * 10.0f * elapsed_time;
	//	
 //       Matrix44 playerRotation;
 //       playerRotation.rotate(player->yaw * DEG2RAD, Vector3(0,1,0));
 //       
 //       Vector3 forward = playerRotation.rotateVector(Vector3(0,0,-1));
 //       Vector3 right = playerRotation.rotateVector(Vector3(1,0,0));
 //       Vector3 playerVel;	
 //       
	//	
	//	if (Input::isKeyPressed(SDL_SCANCODE_W)) { playerVel = playerVel + (playerSpeed * forward); slowMotion = false; }
	//	if (Input::isKeyPressed(SDL_SCANCODE_S)) { playerVel = playerVel - (playerSpeed * forward);  slowMotion = false; }
	//	if (Input::isKeyPressed(SDL_SCANCODE_D)) { playerVel = playerVel + (playerSpeed * right); slowMotion = false; }
	//	if (Input::isKeyPressed(SDL_SCANCODE_A)) { playerVel = playerVel - (playerSpeed * right);  slowMotion = false; }

	//	if (slowMotion) {
	//		elapsed_time *= 0.1f;

	//	}

	//	//Colisions dels Players (player + enemcis)
	//	player->checkColisions(playerVel, entities, elapsed_time, 0.2f);

	//}
	//else {
	//	//async input to move the camera around
	//	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
	//	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	//	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	//	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	//	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	//	if (Input::isKeyPressed(SDL_SCANCODE_E)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	//	if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);

	//}
	//update bala de la posicio i si colisiona amb enemics o parets
	//world.shooting_update(entities, enemies, bullets, player_entity);



	//AI ENEMIES - Canvi de posicio dels enemics  + comprovar colisions enemics
	/*for (size_t i = 0; i < player_enemies.size(); i++){
		Player* enemy = player_enemies[i];
		enemy->AIEnemy(elapsed_time);
	}*/
	
	//to navigate with the mouse fixed in the middle
	/*if (mouse_locked)
		Input::centerMouse();*/
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	GetCurrent()->onKeyDown(event);

	//switch(event.keysym.sym)
	//{
 //       /*case SDLK_ESCAPE: {
	//		SDL_ShowCursor(true);
	//		cameraLocked = !cameraLocked;
 //           pause = !pause;
 //           break;
 //       }*/
	//	case SDLK_F1: Shader::ReloadAll(); break;
	//	case SDLK_1: enemies = world.AddEntityInFront(camera, 6, enemies); break;
 //       case SDLK_2: entities = world.AddEntityInFront(camera, entityToAdd, entities); break;
 //       case SDLK_3: selectedEntity = world.RayPick(camera, points, entities, selectedEntity);
 //           if (selectedEntity == NULL) printf("selected entity not saved!\n"); 
 //           break;
 //       case SDLK_4:  world.RotateSelected(10.0f, selectedEntity); break;
 //       case SDLK_5:  world.RotateSelected(-10.0f, selectedEntity); break;
	//	case SDLK_6:  entities = world.DeleteEntity(camera, points, entities); break;
	//	case SDLK_0: world.saveWorld(entities, enemies); break;
	//		//path finding
	//	case SDLK_7: {
	//		Vector2 mouse = Input::mouse_position;
	//		Game* g = Game::instance;
	//		Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, window_width, window_height);
	//		Vector3 rayOrigin = camera->eye;

	//		Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	//		world.start_x = clamp(spawnPos.x / world.tileSizeX, 0, world.W);
	//		world.start_y = clamp(spawnPos.z / world.tileSizeY, 0, world.H);
	//		//printf("(start_x, start_y) = (%d, %d)\n", start_x, start_y);
	//		break;
	//	}
	//	case SDLK_8: {
	//		Vector2 mouse = Input::mouse_position;
	//		Game* g = Game::instance;
	//		Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, window_width, window_height);
	//		Vector3 rayOrigin = camera->eye;

	//		Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	//		world.target_x = clamp(spawnPos.x / world.tileSizeX, 0, world.W);
	//		world.target_y = clamp(spawnPos.z / world.tileSizeY, 0, world.H);

	//		//printf("(target_x, target_y) = (%d, %d)\n", target_x, target_y);


	//		world.path_steps = AStarFindPathNoTieDiag(
	//			world.start_x, world.start_y, //origin (tienen que ser enteros)
	//			world.target_x, world.target_y, //target (tienen que ser enteros)
	//			world.map_grid, //pointer to map data
	//			world.W, world.H, //map width and height
	//			world.output, //pointer where the final path will be stored
	//			100); //max supported steps of the final path


	//	//check if there was a path
	//		if (world.path_steps != -1)
	//		{
	//			for (int i = 0; i < world.path_steps; ++i)
	//				std::cout << "X: " << (world.output[i] % world.W) << ", Y: " << floor(world.output[i] / world.W) << std::endl;
	//		}
	//		else {
	//			printf("No paths.\n");
	//		}


	//		break;
	//	}
	//	case SDLK_9: world.loadWorld(); break;
	//	case SDLK_PLUS: entityToAdd = (entityToAdd + 1) % 5; //canviar enum sense bullet (enum = 5) i el 6 es el enemic

	//}
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
		if (currentStage == STAGE_ID::TUTORIAL) {

			
			if (!GetCurrent()->player->shot && cameraLocked) { //solament pot disparar quan ha acabat la animaci— de disparar				
				GetCurrent()->player->Shoot(GL_TRIANGLES, camera, shader, cameraLocked, GetCurrent()->playerModel,GetCurrent()->player);
				GetCurrent()->player->shot = true;
			}
		}

		if (currentStage == STAGE_ID::MENU) {
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

void Game::loadTexturesAndMeshes() {
	//BOTONS
	texture_black = texture_black->getBlackTexture();
	texture_white = texture_black->getWhiteTexture();

	play = Texture::Get("data/gui/play_.png");
	restart = Texture::Get("data/gui/restart_.png");
	save = Texture::Get("data/gui/save_.png");
	exit = Texture::Get("data/gui/exit_.png");
	volumeOn = Texture::Get("data/gui/save.png");
	volumeOff = Texture::Get("data/gui/exit.png");


	mesh_sphere = Mesh::Get("data/sphere.obj");

	mesh_ground = new Mesh();
	mesh_ground->createPlane(30);
	texture_ground = Texture::Get("data/ground.png");
	
	mesh_pistol = Mesh::Get("data/pistol.obj");

	mesh_pistol_e = Mesh::Get("data/pistol_enemy.obj");
	texture_pistol  = Texture::Get("data/color-atlas-new.png");

	texture_sky = Texture::Get("data/sky/sky.tga");
	mesh_sky = Mesh::Get("data/sky/sky.ASE");

	
	mesh_bullet = Mesh::Get("data/bullet_hack.obj");
	texture_bullet = Texture::Get("data/bullet_hack.png");

	//meshes per carregar in game
	mesh_rock1 = Mesh::Get("data/rock1.obj");
	texture_rock1 = Texture::Get("data/rock1.png");
	mesh_barrel = Mesh::Get("data/levels/obj_scene/barrel.obj");
	mesh_consoleScreen = Mesh::Get("data/levels/obj_scene/consoleScreen.obj");
	mesh_SupportCorner = Mesh::Get("data/levels/obj_scene/SupportCorner.obj");
	texture_barrel = Texture::Get("data/levels/obj_scene/barrel.png");
	texture_consoleScreen = Texture::Get("data/levels/obj_scene/consoleScreen.png");
	texture_SupportCorner = Texture::Get("data/levels/obj_scene/SupportCorner.png");

	//imports d'arxius d'animació
	mesh_cowboy_run = Mesh::Get("data/animation/cowboy_run.mesh");
	mesh_cowboy_idle = Mesh::Get("data/animation/cowboy_idle.mesh");
	texture_cowboy = Texture::Get("data/animation/westernpack.png");
	anim_run = Animation::Get("data/animation/cowboy_run.skanim");
	anim_idle = Animation::Get("data/animation/cowboy_idle.skanim");
	box_col = Mesh::Get("data/box_colision_enemy.obj");

	//we load a shader
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
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
void Game::PlayGameSound(HSAMPLE fileSample) {
	

	//El handler para un canal
	HCHANNEL hSampleChannel;

	//Creamos un canal para el sample
	hSampleChannel = BASS_SampleGetChannel(fileSample, false);

	//si esteim en SlowMotion, baixam la freq. de mostreig de canal (original = 44100Hz)
	if(GetCurrent()->slowMotion){ BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_FREQ, 15000); }
	else { BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_FREQ, 0); }
	
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, 0.3); //volumen del dispar (el podriem controlar amb una variable des del menu!
	//Lanzamos un sample
	BASS_ChannelPlay(hSampleChannel, true);


}
void Game::LoadAllSamples() {
	shoot = LoadSample("data/audios/paintball.wav");
	recoil = LoadSample("data/audios/recoil.wav");
	hit_enemy = LoadSample("data/audios/hit_enemy.wav");
	hit_player = LoadSample("data/audios/hit_player.wav");
}

//bool Game::RenderButton(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV ) {
//	Vector2 mouse = Input::mouse_position;
//	float halfWidth = w * 0.5;
//	float halfHeight = h * 0.5;
//	float min_x = x - halfWidth;
//	float max_x = x + halfWidth;
//	float min_y = y - halfHeight;
//	float max_y = y + halfHeight;
//
//	bool hover = mouse.x >= min_x && mouse.x <= max_x && mouse.y >= min_y && mouse.y <= max_y;
//	Vector4 buttonColor = hover ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.7f);
//
//	RenderGUI(x, y, w, h, texture, buttonColor, flipYV);
//	return wasLeftMousePressed && hover;
//}

//void Game::RenderGUI(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV) {
//	int window_width = Game::instance->window_width;
//	int window_height = Game::instance->window_height;
//	Mesh quad;
//	quad.createQuad(x, y, w, h, flipYV);
//
//	Camera cam2D;
//	cam2D.setOrthographic(0, window_width, window_height, 0, -1, 1);
//
//	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
//	//Texture* texture = Texture::Get("data/gui/play-button.png");
//
//	if (!shader) return;
//	shader->enable();
//
//	shader->setUniform("u_color", color);
//	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
//	if (texture != NULL) {
//		shader->setUniform("u_texture", texture, 0);
//	}
//	shader->setUniform("u_time", time);
//	//shader->setUniform("u_tex_tiling", 1.0f);
//	shader->setUniform("u_model", Matrix44());
//	quad.render(GL_TRIANGLES);
//
//	shader->disable();
//}
Stage* Game::GetStage(STAGE_ID id) {
	return stages[(int)id];
}
Stage* Game::GetCurrent() {
	return GetStage(currentStage);
}
void Game::SetStage(STAGE_ID id) {
	currentStage = id;
}


void Game::InitStages() {
	stages.reserve(6);
	stages.push_back(new Intro());
	stages.push_back(new Tutorial());
	stages.push_back(new Level());
	stages.push_back(new Final());
	stages.push_back(new EditMode());
	stages.push_back(new Menu());
}