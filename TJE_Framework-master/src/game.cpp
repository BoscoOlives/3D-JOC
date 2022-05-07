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

Texture* texture = NULL;
Texture* texture_island = NULL;
Texture* texture_plane = NULL;
Texture* texture_bomb = NULL;
Texture* texture_car = NULL;
Matrix44 planeModel; // NO HA DE QUEDAR AQUI
Matrix44 bombModel;
Matrix44 bombOffset;
bool cameraLocked = true;
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


class Entity {
public:
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;
};
std::vector<Entity*> entities;

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

void RenderMesh(Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam) {
	assert((a_mesh != NULL, "mesh in renderMesh was null"));
	if (!a_shader) return;
	
	float time = Game::instance->time;
	//enable shader
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	a_shader->setUniform("u_texture", tex, 0);
	a_shader->setUniform("u_time", time);
	a_shader->setUniform("u_model", model);
	a_mesh->render(GL_TRIANGLES);

	a_shader->disable();
}

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

void AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture) {
	Vector2 mousePos = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	
	Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);


	Entity* entity = new Entity;
	entity->model = model;
	entity->mesh = mesh;
	entity->texture = texture;
	entities.push_back(entity);
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

	//create model matrix for cube
	//Matrix44 m;
	//m.rotate(angle*DEG2RAD, Vector3(0, 1, 0));

	//Matrix44 m2;
	//m2.translate(150, 0, 0);
	//m2.rotate(angle * DEG2RAD, Vector3(0, 1, 0));
	//m2.scale(100, 100, 100);
	
	if (cameraLocked) {
		Vector3 eye = planeModel * Vector3(0.0f, 9.0f, 16.0f);
		Vector3 center = planeModel * Vector3(0.0f, 0.0f, -20.0f);
		Vector3 up = planeModel.rotateVector(Vector3(0.0f, 1.0f, 0.0f));	
		camera->lookAt(eye, center, up);
	}
	
	//RENDER ISLANDS
	//RenderIslands();
	Matrix44 islandModel;
	RenderMesh(islandModel, mesh_island, texture_island, shader, camera);
	mesh_island->renderBounding(islandModel);
	//Render PLANE		
	//RenderMesh(planeModel, mesh_plane, texture_plane, shader, camera);
    //RenderMesh(bombModel, mesh_bomb, texture_bomb, shader, camera);
    RenderPlanes();

	
	for (size_t i = 0; i < entities.size(); i++) { //Renderitza totes les entitats que es creen, ARA MATEIX NOMES CREEM ELS CARROS AMB LA TECLA 2
		Entity* entity = entities[i];
		RenderMesh(entity->model, entity->mesh, entity->texture, shader, camera);
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

	if (cameraLocked) {
		float planeSpeed = 50.0f * elapsed_time;
		float rotSpeed = 90.0f * DEG2RAD * elapsed_time;
		if (Input::isKeyPressed(SDL_SCANCODE_W) ) planeModel.translate(0.0f, 0.0f, -planeSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) planeModel.translate(0.0f, 0.0f, planeSpeed);

		
		if (Input::isKeyPressed(SDL_SCANCODE_A)) planeModel.rotate(-rotSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_D)) planeModel.rotate(rotSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_E)) planeModel.rotate(rotSpeed, Vector3(0.0f, 0.0f, 1.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) planeModel.rotate(-rotSpeed, Vector3(0.0f, 0.0f, 1.0f));
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
		case SDLK_2: AddEntityInFront(camera, mesh_car, texture_car); break; // amb la tecla 2 creem ENTITATS on estigui el mouse. 
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

