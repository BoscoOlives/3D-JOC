#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"





#include <cmath>

//#define EDITOR

//some globals
Mesh* mesh = NULL;

Mesh* mesh_man = NULL;

Texture* texture = NULL;


bool cameraLocked = false;
bool bombAttached = true;

Shader* shader = NULL;

Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;


Game* Game::instance = NULL;

const int planes_width = 200;
const int planes_height = 200;
float padding = 20.0f;

float lod_distance = 200.0f;
float no_render_distance = 1000.0f;


Mesh* mesh_ground;
Texture* texture_ground;

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

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
    
	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective
    
    
    
    mesh_ground = new Mesh();
    mesh_ground->createPlane(1000);
    texture_ground = Texture::Get("data/ground.tga");
	
	//EXMPLE FETS A CLASSE, HAURIEN DE SER ENTITIES

	mesh_house = Mesh::Get("data/bar-tropic_0.obj");
	mesh_cube = Mesh::Get("data/box.ASE");
	mesh_man = Mesh::Get("data/man.obj");
	texture_cube = Texture::Get("data/sptifire/sptifire_low_flat.tga");

	texture_black = texture_black->getBlackTexture();
	texture_white = texture_black->getWhiteTexture();
	
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");	

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
    
    Texture* texture_sky = Texture::Get("data/sky/sky.tga");
    Mesh* mesh_sky = Mesh::Get("data/sky/sky.ASE");
    Matrix44 skyModel;
    skyModel.translate(camera->eye.x, camera->eye.y - 40.0f, camera->eye.z);
    Entity background = Entity(skyModel, mesh_sky, texture_sky);
    glDisable(GL_DEPTH_TEST); //desactivem el DEPTH TEST abans de redenritzar el fons perque no es superposi devant entitats
    background.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
    glEnable(GL_DEPTH_TEST);
    
    Entity ground = Entity(Matrix44(), mesh_ground, texture_ground);
    ground.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
	
	
	//Matrix44 m;
	//m.rotate(angle*DEG2RAD, Vector3(0, 1, 0));

	//Matrix44 m2;
	//m2.translate(150, 0, 0);
	//m2.rotate(angle * DEG2RAD, Vector3(0, 1, 0));
	//m2.scale(100, 100, 100);
	Matrix44 playerModel;
    playerModel.translate(player->pos.x, player->pos.y, player->pos.z);
	playerModel.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
	playerModel.rotate(player->yaw * DEG2RAD, Vector3(0, 1, 0));
	


	if (cameraLocked) { //en aquet cas hi ha la possibilitat de les dues vistes, pero si ens quedem amb un FirstPerson, sobra la mitad de aqest if

		Matrix44 camModel = playerModel;
		camModel.rotate(180 * DEG2RAD, Vector3(0, 1, 0));// Girem la camera perque sinos apunta cap al darrera
		camModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));

		Vector3 eye = playerModel * Vector3(0, 1, -0.5);
		Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
		Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));
		
              
		camera->lookAt(eye, center, up);
	}
	
    
    //CREAR JUGADOR
	Entity player_entity = Entity(playerModel, mesh_man, texture_black);

	player_entity.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
    
    
	for (size_t i = 0; i < entities.size(); i++) { //Renderitza totes les entitats que es creen, ARA MATEIX NOMES CREEM ELS CARROS AMB LA TECLA 2
		Entity* entity = entities[i];
		entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
		//RenderMesh(entity->model, entity->mesh, entity->texture, shader, camera);
	}
//	Mesh m;
//	m.vertices = points;
//	Entity* point = new Entity(Matrix44(), &m, NULL);
//	glPointSize(4.0f);
//	point->RenderEntity(GL_POINTS, Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs"), camera, cameraLocked);
//	glPointSize(1.0f);


	if (!cameraLocked) {//TEXT TECLES MODE EDICIÓ
		std::string text_edicio = "F1 Reload All\n 0 Save World\n 2 Add Entity\n 3 Select Entity\n 4 Rotate <-\n 5 Rotate ->\n 6 Remove Entity\n 9 Load World\n";
		drawText(this->window_width-200, 2, text_edicio, Vector3(1, 1, 1), 2);
	}



	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
    if (slowMotion) {
        elapsed_time *= 0.1f;
    }
    
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
    
    if (Input::wasKeyPressed(SDL_SCANCODE_P)) {
        slowMotion = !slowMotion;
        if (slowMotion) printf("slow motion\n");
        else printf("normal speed\n");
    }
    
	SDL_ShowCursor(!cameraLocked);
	if (cameraLocked) { //moviment player
		float playerSpeed = 20.0f * elapsed_time;
		float rotSpeed = 150.0f * elapsed_time;
        
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
        
        if (Input::isKeyPressed(SDL_SCANCODE_W)) playerVel = playerVel + (playerSpeed * forward);
        if (Input::isKeyPressed(SDL_SCANCODE_S)) playerVel = playerVel - (playerSpeed * forward);
        if (Input::isKeyPressed(SDL_SCANCODE_D)) playerVel = playerVel + (playerSpeed * right);
        if (Input::isKeyPressed(SDL_SCANCODE_A)) playerVel = playerVel - (playerSpeed * right);

		Vector3 nexPos = player->pos + playerVel;
		//calculamos el centro de la esfera de colisión del player elevandola hasta la cintura
		Vector3 character_center = nexPos + Vector3(0, 1, 0);

		for (size_t i = 0; i < entities.size(); i++)
		{
			Entity* currentEntity = entities[i];

			Vector3 coll;
			Vector3 collnorm;
			//comprobamos si colisiona el objeto con la esfera (radio 3)
			if (!currentEntity->mesh->testSphereCollision(currentEntity->model, character_center, 3, coll, collnorm))
				continue; //si no colisiona, pasamos al siguiente objeto

			//si la esfera está colisionando muevela a su posicion anterior alejandola del objeto
			Vector3 push_away = normalize(coll - character_center) * elapsed_time;
			nexPos = player->pos - push_away; //move to previous pos but a little bit further

			//cuidado con la Y, si nuestro juego es 2D la ponemos a 0
			nexPos.y = 0;

			//reflejamos el vector velocidad para que de la sensacion de que rebota en la pared
			//velocity = reflect(velocity, collnorm) * 0.95;
		}


        player->pos = nexPos;

		//Generem una bala / bullet
		
		if (Input::wasKeyPressed(SDL_SCANCODE_SPACE)) {
			entities = player->Shot(GL_TRIANGLES, camera, shader, cameraLocked, entities);
			
		}

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
    
	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];
		if (entity->current_entity == Entity::ENTITY_ID::BULLET) { //render de les bales
			//Matrix44 model_bullet;
            Bullet* bullet = (Bullet*)(entity);
            bullet->update_position(elapsed_time);
		}
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
        case SDLK_2: entities = world.AddEntityInFront(camera, entityToAdd, entities); break;
        case SDLK_3: selectedEntity = world.RayPick(camera, points, entities, selectedEntity);
            if (selectedEntity == NULL) printf("selected entity not saved!\n"); 
            break;
        case SDLK_4:  world.RotateSelected(10.0f, selectedEntity); break;
        case SDLK_5:  world.RotateSelected(-10.0f, selectedEntity); break;
		case SDLK_6:  entities = world.DeleteEntity(camera, points, entities, selectedEntity); break;
		case SDLK_0: world.saveWorld(entities); break;
		case SDLK_9: entities = world.loadWorld(entities); break;
		case SDLK_PLUS: entityToAdd = (entityToAdd + 1) % 2; //canviar enum sense bullet (enum = 2)
			

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
