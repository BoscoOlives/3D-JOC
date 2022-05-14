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
Mesh* mesh2 = NULL;
Mesh* mesh_island = NULL;
Mesh* mesh_plane = NULL;
Mesh* mesh_bomb = NULL;
Mesh* mesh_car = NULL;
Mesh* mesh_penguin = NULL;

Texture* texture = NULL;
Texture* texture_island = NULL;
Texture* texture_plane = NULL;
Texture* texture_bomb = NULL;
Texture* texture_car = NULL;
Texture* texture_penguin = NULL;
Matrix44 planeModel; // NO HA DE QUEDAR AQUI
Matrix44 bombModel;
Matrix44 bombOffset;
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


class Prop { //SERVIRA PER EXPORTAR EN UN .TXT TOTA LA INFO DE ON GENEREM LES ENTITATS I UN IDENTIFICADOR
	int id;
	Mesh* mesh;
	Texture* texture;
};
Prop props[20];


//class Entity {
//public:
//	Matrix44 model;
//	Mesh* mesh;
//	Texture* texture;
//};

struct Player {
    Vector3 pos;
    float yaw;
};
Player player;

const bool firstPerson = true;

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

    bombOffset.setTranslation(0.0f, -2.0f, 0.0f);
    
	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,100000.f); //set the projection, we want to be perspective
    
    
    
    mesh_ground = new Mesh();
    mesh_ground->createPlane(1000);
    texture_ground = Texture::Get("data/ground.tga");

	//load one texture without using the Texture Manager (Texture::Get would use the manager)
	//texture = new Texture();
 	//texture->load("data/texture.tga");

	// example of loading Mesh from Mesh Manager
	//mesh = Mesh::Get("data/box.ASE");
	//mesh2 = Mesh::Get("data/sphere.obj");
	
	//EXMPLE FETS A CLASSE, HAURIEN DE SER ENTITIES
	texture_island =Texture::Get("data/island/island_color.tga");
	mesh_island = Mesh::Get("data/island/island.ASE");

	texture_plane = Texture::Get("data/spitfire/spitfire_color_spec.tga");
	mesh_plane = Mesh::Get("data/spitfire/spitfire.ASE");

    texture_bomb = Texture::Get("data/torpedo_bullet/torpedo.tga");
    mesh_bomb = Mesh::Get("data/torpedo_bullet/torpedo.ASE");

	texture_car = Texture::Get("data/cart.png");
	mesh_car = Mesh::Get("data/cart.obj");
    
    texture_penguin = Texture::Get("data/color-atlas-new.png");
    mesh_penguin =Mesh::Get("data/penguin_20.obj");
    
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}


void RenderPlanes()
{
    //enable shader
    shader->enable();
    Camera* cam = Game::instance->camera;
    float time = Game::instance->time;
    //upload uniforms
    shader->setUniform("u_color", Vector4(1, 1, 1, 1));
    shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
    shader->setUniform("u_texture", texture_plane, 0);
    shader->setUniform("u_time", time);

    Matrix44 m;
    for (size_t i = 0; i < planes_width; i++)
    {
        for (size_t j = 0; j < planes_height; j++)
        {
            Matrix44 model;
            model.translate(i * padding, 0.0f, j * padding);
            
            Vector3 planePos = model.getTranslation();
            
            if(!cam->testPointInFrustum(planePos)) {
                continue;
            }
            
            Vector3 camPos = cam->eye;
            
            float dist = planePos.distance(camPos);
            
            if(dist > no_render_distance) {
                continue;
            }
            
            Mesh* mesh = Mesh::Get("data/spitfire/spitfire_low.ASE");
            if(dist < lod_distance) {
                mesh = Mesh::Get("data/spitfire/spitfire_low.ASE");
            }
            
            BoundingBox worldAABB = transformBoundingBox(model, mesh->box);
            if (!cam->testBoxInFrustum(worldAABB.center, worldAABB.halfsize)) {
                continue;
            }
            
            shader->setUniform("u_model", model);
            mesh->render(GL_TRIANGLES);
        }
    }

    //disable shader
    shader->disable();
}

//void RenderMesh(Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam) {
//	assert((a_mesh != NULL, "mesh in renderMesh was null"));
//	if (!a_shader) return;
//
//	float time = Game::instance->time;
//	//enable shader
//	a_shader->enable();
//
//	//upload uniforms
//	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
//	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
//	a_shader->setUniform("u_texture", tex, 0);
//	a_shader->setUniform("u_time", time);
//	a_shader->setUniform("u_model", model);
//	a_mesh->render(GL_TRIANGLES);
//
//	a_shader->disable();
//}

void RenderIslands() {

	if (shader)
	{
		//enable shader
		shader->enable();
		Camera* cam = Game::instance->camera;
		float time = Game::instance->time;
		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
		shader->setUniform("u_texture", texture_island, 0);
		shader->setUniform("u_time", time);

		Matrix44 m;
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{	
				Vector3 size = mesh_island->box.halfsize * 2;
				m.setTranslation(size.x * i, 0.0f, size.z * j);
				shader->setUniform("u_model", m);
				mesh_island->render(GL_TRIANGLES);
			}
		}

		//disable shader
		shader->disable();
	}
}

//what to do when the image has to be draw
int hola = 0;
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
    glDisable(GL_DEPTH_TEST);
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
	
	if (cameraLocked) {
		Vector3 eye = playerModel * Vector3(0,3,3);
		Vector3 center = playerModel * Vector3(0,0,-5);
        Vector3 up = Vector3(0,1,0);
        
        if (hola%300 == 0) printf("eye: (%f, %f, %f), center: (%f, %f, %f)\n", eye.x, eye.y, eye.z, center.x, center.y, center.z);
        
		camera->lookAt(eye, center, up);
	}
	
	//RENDER ISLANDS
	//RenderIslands();
	/*Matrix44 islandModel;
	RenderMesh(islandModel, mesh_island, texture_island, shader, camera);
	mesh_island->renderBounding(islandModel);*/
	//Render PLANE		
	//RenderMesh(planeModel, mesh_plane, texture_plane, shader, camera);
    //RenderMesh(bombModel, mesh_bomb, texture_bomb, shader, camera);
    //RenderPlanes();
    
    //CREAR JUGADOR
    //PlayerModel creat al game.h
    playerModel.translate(player.pos.x, player.pos.y, player.pos.z);
    playerModel.rotate(player.yaw * DEG2RAD, Vector3(0, 1, 0));
    Entity player = Entity(playerModel, mesh_plane, texture_plane);
    player.RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
    
    
	for (size_t i = 0; i < entities.size(); i++) { //Renderitza totes les entitats que es creen, ARA MATEIX NOMES CREEM ELS CARROS AMB LA TECLA 2
		Entity* entity = entities[i];
		entity->RenderEntity(GL_TRIANGLES, shader, camera, cameraLocked);
		//RenderMesh(entity->model, entity->mesh, entity->texture, shader, camera);
	}
	Mesh m;
	m.vertices = points;
	Entity* point = new Entity(Matrix44(), &m, NULL);
	glPointSize(4.0f);
	point->RenderEntity(GL_POINTS, Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs"), camera, cameraLocked);
	glPointSize(1.0f);


	//Draw the floor grid
	drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
    
    hola++;
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		
		cameraLocked = !cameraLocked;
	}

	if (cameraLocked) { //moviment player
		float playerSpeed = 8.0f * elapsed_time;
		float rotSpeed = 200.0f * DEG2RAD * elapsed_time;
        
        if (Input::isKeyPressed(SDL_SCANCODE_E)) player.yaw = player.yaw + rotSpeed;
        if (Input::isKeyPressed(SDL_SCANCODE_Q)) player.yaw = player.yaw - rotSpeed;
        
        Matrix44 playerRotation;
        playerRotation.rotate(player.yaw * DEG2RAD, Vector3(0,1,0));
        
        Vector3 forward = playerRotation.rotateVector(Vector3(0,0,-1));
        Vector3 right = playerRotation.rotateVector(Vector3(1,0,0));
        Vector3 playerVel;
        
        if (Input::isKeyPressed(SDL_SCANCODE_W)) playerVel = playerVel + (playerSpeed * forward);
        if (Input::isKeyPressed(SDL_SCANCODE_S)) playerVel = playerVel - (playerSpeed * forward);
        if (Input::isKeyPressed(SDL_SCANCODE_A)) playerVel = playerVel + (playerSpeed * right);
        if (Input::isKeyPressed(SDL_SCANCODE_D)) playerVel = playerVel - (playerSpeed * right);

        player.pos = player.pos + playerVel;
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
    
    if (Input::wasKeyPressed(SDL_SCANCODE_F))
    {
        bombAttached = false;
    }
    
    if (bombAttached)
    {
        bombModel = bombOffset * planeModel;
    }
    else
    {
        bombModel.translateGlobal(0.0f, -9.8f * elapsed_time, 0.0f);
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
        case SDLK_2: entities = AddEntityInFront(camera, mesh_plane, texture_plane, entities); break;
        case SDLK_3: selectedEntity = RayPick(camera, points, entities, selectedEntity);
            if (selectedEntity == NULL) printf("selected entity not saved!\n"); 
            break;
        case SDLK_4: RotateSelected(10.0f, selectedEntity); break;
        case SDLK_5: RotateSelected(-10.0f, selectedEntity); break;
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

