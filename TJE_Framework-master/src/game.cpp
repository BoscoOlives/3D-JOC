#include "game.h"
#include "utils.h"
#include "input.h"






#include <cmath>

//#define EDITOR

Game* Game::instance = NULL;


float lod_distance = 200.0f;
float no_render_distance = 1000.0f;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;
	cameraLocked = false;
	slowMotion = false;
	angle = 0;
	mouse_speed = 100.0f;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
    
	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,50.f, 50.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective
    
	//Load Mesh i Textures
	loadTexturesAndMeshes();

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	anim_shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");

	//pathfinding
	world.creteGrid();
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

	//set the camera as default
	

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
   
	camera->enable();
    
    
    Matrix44 skyModel;
    skyModel.translate(camera->eye.x, camera->eye.y - 40.0f, camera->eye.z);
    Entity background = Entity(skyModel, mesh_sky, texture_sky);
    glDisable(GL_DEPTH_TEST); //desactivem el DEPTH TEST abans de redenritzar el fons perque no es superposi devant entitats
    background.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
    glEnable(GL_DEPTH_TEST);
    
    Entity ground = Entity(Matrix44(), mesh_ground, texture_ground);
    ground.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
	
	//-------------------RENDER DEL PUNT DE COLISIO!-------------------
	/*Matrix44 model_colision;
	model_colision.setTranslation(character_center.x, character_center.y, character_center.z);
	Entity* ent_colision = new Entity(model_colision, mesh_bullet, texture_black);
	ent_colision->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);*/
	//-----------------------------------------------------------------

	//-------------------RENDER DEL PUNT DE COLISIO ENEMICS!-------------------
	for (size_t i = 0; i < player_enemies.size(); i++)
	{
		Vector3 pos = player_enemies[i]->pos;
		Matrix44 model_colision;
		model_colision.setTranslation(pos.x, pos.y, pos.z);

		Entity* ent_colision = new Entity(model_colision, mesh_bullet, texture_black);
		ent_colision->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
	}
	//-----------------------------------------------------------------

    //playerModel.translate(player->pos.x, player->pos.y, player->pos.z);
	playerModel.setTranslation(player->pos.x, player->pos.y, player->pos.z);
	//playerModel.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
	//playerModel.rotate(180 * DEG2RAD, Vector3(1, 0, 0));
	playerModel.rotate(player->yaw * DEG2RAD, Vector3(0, 1, 0));
	playerModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));



	if (cameraLocked) {

		Matrix44 camModel = playerModel;
		//camModel.rotate(180 * DEG2RAD, Vector3(0, 1, 0));// Girem la camera perque sinos apunta cap al darrera
		//camModel.rotate(180 * DEG2RAD, Vector3(1, 0, 0));// Girem la camera perque sinos apunta cap al darrera
		//camModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));


		Vector3 eye = playerModel * Vector3(0, 0.7, 0.5); //segon valor altura camera
		//Vector3 eye = world.Lerp(camera->eye, desiredeEye, 100.f * elapsed_time); //Lerp perque es vegi sa pistola una mica de costat quan ens movem
		Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
		Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));
		
		camera->lookAt(eye, center, up);
	}
	
	if (player->shoot) { //moviment que provoca un shot a la arma
		playerModel = player->Coil(elapsed_time, playerModel);
	}
    //CREAR JUGADOR
	Entity player_entity = Entity(playerModel, mesh_pistol, texture_black);
	//render Player
	player_entity.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
    
    //render de totes les entitats (estatiques)
	for (size_t i = 0; i < entities.size(); i++) { //Renderitza totes les entitats que es creen
		Entity* entity = entities[i];
		entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
	}
	//render de tots els enemics
	for (size_t i = 0; i < enemies.size(); i++) { //Renderitza tots els enemics que es creen
		Entity* entity = enemies[i];
		Player* enemy = player_enemies[i];
		entity->RenderEntityAnim(GL_TRIANGLES, anim_shader, camera, enemy->pos, enemy->yaw);
	}

	if (!cameraLocked) {//TEXT TECLES MODE EDICI�
		std::string text_edicio = "F1 Reload All\n 0 Save World\n 2 Add Entity\n 3 Select Entity\n 4 Rotate <-\n 5 Rotate ->\n 6 Remove Entity\n 9 Load World\n + Change Entity to Add\n";
		drawText(this->window_width-200, 2, text_edicio, Vector3(1, 1, 1), 2);
	}
	if (cameraLocked) {//TEXT TECLES MODE GAMEPLAY
		std::string text_gameplay = "SPACE Shot\nWASD Move Player\nMouse Move Camera\n";
		drawText(this->window_width - 200, 2, text_gameplay, Vector3(1, 1, 1), 2);
	}

	//Pathfinding
	//world.renderPath();


	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	slowMotion = true;    
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam 
	if (!cameraLocked) {
		if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed? NO ENTENC PER OR DE MOUSE_LOCKED
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, Vector3(-1.0f,0.0f,0.0f));
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		
		cameraLocked = !cameraLocked;
	}
    
	SDL_ShowCursor(!cameraLocked);
	if (cameraLocked) { //moviment player
		float playerSpeed = 3.0f * elapsed_time;
		float rotSpeed = 120.0f * elapsed_time;
        
        //if (Input::isKeyPressed(SDL_SCANCODE_D)) player->yaw = player->yaw + rotSpeed;
        //if (Input::isKeyPressed(SDL_SCANCODE_A)) player->yaw = player->yaw - rotSpeed;
		
		Input::centerMouse();
		player->pitch += -Input::mouse_delta.y * 10.0f * elapsed_time;
		player->yaw += -Input::mouse_delta.x * 10.0f * elapsed_time;
		
        Matrix44 playerRotation;
        playerRotation.rotate(player->yaw * DEG2RAD, Vector3(0,1,0));
        
        Vector3 forward = playerRotation.rotateVector(Vector3(0,0,-1));
        Vector3 right = playerRotation.rotateVector(Vector3(1,0,0));
        Vector3 playerVel;	
        
		
		if (Input::isKeyPressed(SDL_SCANCODE_W)) { playerVel = playerVel + (playerSpeed * forward); slowMotion = false; }
		if (Input::isKeyPressed(SDL_SCANCODE_S)) { playerVel = playerVel - (playerSpeed * forward);  slowMotion = false; }
		if (Input::isKeyPressed(SDL_SCANCODE_D)) { playerVel = playerVel + (playerSpeed * right); slowMotion = false; }
		if (Input::isKeyPressed(SDL_SCANCODE_A)) { playerVel = playerVel - (playerSpeed * right);  slowMotion = false; }

		if (slowMotion) {
			elapsed_time *= 0.01f;
		}

		Vector3 nexPos = player->pos + playerVel;
		//calculamos el centro de la esfera de colisi�n del player elevandola hasta la cintura
		character_center = nexPos + Vector3(0, 0.5, 0);

		for (size_t i = 0; i < entities.size(); i++)
		{
			Entity* currentEntity = entities[i];

			Vector3 coll;
			Vector3 collnorm;
			//comprobamos si colisiona el objeto con la esfera (radio 3)
			if (!currentEntity->mesh->testSphereCollision(currentEntity->model, character_center, 0.2, coll, collnorm))
				continue; //si no colisiona, pasamos al siguiente objeto

			//si la esfera est� colisionando muevela a su posicion anterior alejandola del objeto
			Vector3 push_away = normalize(coll - character_center) * elapsed_time;
			nexPos = player->pos - push_away; //move to previous pos but a little bit further

			//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
			nexPos.y = 0;

			//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
			//velocity = reflect(velocity, collnorm) * 0.95;
		}
        player->pos = nexPos;

	}
	else {
		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT) ) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_E)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);

	}
	//Generem una bala / bullet
	if (Input::wasKeyPressed(SDL_SCANCODE_SPACE) && !player->shoot) { //solament pot disparar quan ha acabat la animaci� de disparar
		entities = player->Shot(GL_TRIANGLES, camera, shader, cameraLocked, entities);
		player->shoot = true;
	}
	//update bala de la posicio i si colisiona amb enemics o parets
	world.shooting_update(entities, enemies);

	//AI ENEMIES 
	for (size_t i = 0; i < player_enemies.size(); i++){
		Player* enemy = player_enemies[i];
		enemy->AIEnemy(seconds_elapsed);
	}
	
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break;
		case SDLK_1: enemies = world.AddEntityInFront(camera, 6, enemies); break;
        case SDLK_2: entities = world.AddEntityInFront(camera, entityToAdd, entities); break;
        case SDLK_3: selectedEntity = world.RayPick(camera, points, entities, selectedEntity);
            if (selectedEntity == NULL) printf("selected entity not saved!\n"); 
            break;
        case SDLK_4:  world.RotateSelected(10.0f, selectedEntity); break;
        case SDLK_5:  world.RotateSelected(-10.0f, selectedEntity); break;
		case SDLK_6:  entities = world.DeleteEntity(camera, points, entities); break;
		case SDLK_0: world.saveWorld(entities); break;
			//path finding
		case SDLK_7: {
			Vector2 mouse = Input::mouse_position;
			Game* g = Game::instance;
			Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
			Vector3 rayOrigin = camera->eye;

			Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
			world.start_x = clamp(spawnPos.x / world.tileSizeX, 0, world.W);
			world.start_y = clamp(spawnPos.z / world.tileSizeY, 0, world.H);
			//printf("(start_x, start_y) = (%d, %d)\n", start_x, start_y);
			break;
		}
		case SDLK_8: {
			Vector2 mouse = Input::mouse_position;
			Game* g = Game::instance;
			Vector3 dir = camera->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
			Vector3 rayOrigin = camera->eye;

			Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
			world.target_x = clamp(spawnPos.x / world.tileSizeX, 0, world.W);
			world.target_y = clamp(spawnPos.z / world.tileSizeY, 0, world.H);

			//printf("(target_x, target_y) = (%d, %d)\n", target_x, target_y);


			world.path_steps = AStarFindPathNoTieDiag(
				world.start_x, world.start_y, //origin (tienen que ser enteros)
				world.target_x, world.target_y, //target (tienen que ser enteros)
				world.map_grid, //pointer to map data
				world.W, world.H, //map width and height
				world.output, //pointer where the final path will be stored
				100); //max supported steps of the final path


		//check if there was a path
			if (world.path_steps != -1)
			{
				for (int i = 0; i < world.path_steps; ++i)
					std::cout << "X: " << (world.output[i] % world.W) << ", Y: " << floor(world.output[i] / world.W) << std::endl;
			}
			else {
				printf("No paths.\n");
			}


			break;
		}
		case SDLK_9: entities = world.loadWorld(entities); break;
		case SDLK_PLUS: entityToAdd = (entityToAdd + 1) % 5; //canviar enum sense bullet (enum = 5) i el 6 es el enemic
			

	}
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
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
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
	mesh_ground = new Mesh();
	mesh_ground->createPlane(1000);
	texture_ground = Texture::Get("data/ground.tga");

	mesh_house = Mesh::Get("data/bar-tropic_0.obj");

	mesh_wall = Mesh::Get("data/wall.obj");
	texture_wall = Texture::Get("data/wall.png");

	mesh_man = Mesh::Get("data/man.obj");
	
	mesh_pistol = Mesh::Get("data/pistol.obj");
	mesh_pistol_e = Mesh::Get("data/pistol_enemy.obj");
	
	mesh_ring = Mesh::Get("data/tanca.obj");
	texture_ring = Texture::Get("data/tancarParets.png");

	texture_sky = Texture::Get("data/sky/sky.tga");
	mesh_sky = Mesh::Get("data/sky/sky.ASE");

	mesh_zona0 = Mesh::Get("data/zona_0.obj");
	texture_zona0 = Texture::Get("data/zona_0.png");

	mesh_zona1 = Mesh::Get("data/zona_1.obj");
	texture_zona1 = Texture::Get("data/zona_1.png");
	
	mesh_bullet = Mesh::Get("data/bullet.obj");
	texture_bullet = Texture::Get("data/bullet.png");

	//imports d'arxius d'animaci�
	mesh_cowboy_run = Mesh::Get("data/animation/cowboy_run.mesh");
	mesh_cowboy_walk = Mesh::Get("data/animation/cowboy_walk.mesh");
	texture_cowboy = Texture::Get("data/animation/westernpack.png");
	anim_run = Animation::Get("data/animation/cowboy_run.skanim");
	anim_walk = Animation::Get("data/animation/cowboy_walk.skanim");


	texture_black = texture_black->getBlackTexture();
	texture_white = texture_black->getWhiteTexture();
}
