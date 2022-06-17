//
//  stage.cpp
//  TJE_XCODE
//
//  Created by Josep Ricci on 10/6/22.
//  Copyright © 2022 Josep Ricci. All rights reserved.
//

#include "stage.h"
#include "game.h"
World Stage::world;
//Stage::Stage() {
//
//}

Intro::Intro() {
 
}
void Intro::Render(bool cameraLocked) {

}
void Intro::Update(float seconds_elapsed, bool &cameraLocked) {

}
void Intro::onKeyDown(SDL_KeyboardEvent event) {

}

Tutorial::Tutorial() {
	Game* g = Game::instance;
	slowMotion = false;
	player->enemy = false;
	//CREAR JUGADOR
	
	player_entity = new Entity(playerModel, g->mesh_pistol, g->texture_black); //creem la entitat Jugador
	world.restartWorld();

}
void Tutorial::Render(bool cameraLocked) {
	Game* g = Game::instance;
	Camera* camera = g->camera;
	
	renderSkyGround(camera, cameraLocked);

	playerModel.setTranslation(player->pos.x, player->pos.y, player->pos.z);
	playerModel.rotate(player->yaw * DEG2RAD, Vector3(0, 1, 0));
	playerModel.rotate(player->pitch * DEG2RAD, Vector3(1, 0, 0));

	if (cameraLocked) {	
		Matrix44 camModel = playerModel;
	
		Vector3 eye = playerModel * Vector3(0, 0.7, 0.5); //segon valor altura camera
		//Vector3 eye = world.Lerp(camera->eye, desiredeEye, 100.f * elapsed_time); //Lerp perque es vegi sa pistola una mica de costat quan ens movem
		Vector3 center = eye + camModel.rotateVector(Vector3(0, 0, -1));
		Vector3 up = camModel.rotateVector(Vector3(0, 1, 0));
			
		camera->lookAt(eye, center, up);
	}
	playerModel.translate(0.1, 0, 0);

	if (player->shot) { //moviment que provoca un shot a la arma
		playerModel = player->Coil(g->elapsed_time, playerModel);

	}

	//CREAR JUGADOR
	player_entity->model = playerModel;
	player_entity->RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
	//Render de la nostra colisio!
	/*Entity* box = new Entity(playerModel, g->box_col, g->texture_black);
	box->RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);*/

	//render de totes les entitats (estatiques)
	for (size_t i = 0; i < world.entities.size(); i++) { //Renderitza totes les entitats que es creen
		Entity* entity = world.entities[i];
		entity->RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
	}

	//render de tots els enemics
	for (size_t i = 0; i < world.enemies.size(); i++) { //Renderitza tots els enemics que es creen
		Entity* entity = world.enemies[i];
		Player* enemy = world.player_enemies[i];
		entity->RenderEntityAnim(GL_TRIANGLES, g->anim_shader, camera, enemy->pos, enemy->yaw, enemy->look, slowMotion,cameraLocked);

		//render Colision BOX ENEMY
		Matrix44 box_model;
		box_model.setTranslation(entity->model.getTranslation().x, entity->model.getTranslation().y, entity->model.getTranslation().z);
		Entity* box = new Entity(box_model, g->box_col, g->texture_black);
		box->RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
	}

	//render de totes les bales
	for (size_t i = 0; i < world.bullets.size(); i++) { //Renderitza totes les bales que es creen
		Entity* entity = world.bullets[i];
		entity->RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
	}
	if (cameraLocked) {//TEXT TECLES MODE GAMEPLAY
		std::string text_gameplay = "LeftMouse Shot\nWASD Move Player\nMouse Move Camera\n ESC Menu\n";
		drawText(g->window_width - 200, 2, text_gameplay, Vector3(1, 1, 1), 2);
		drawText(g->window_width / 2, g->window_height / 2, "+", Vector3(1, 1, 1), 2);
		std:string num_enemies = "N. Enemies" + to_string((unsigned int)world.enemies.size());
		drawText(2, 20, num_enemies, Vector3(1, 1, 1), 2);
	}

}
void Tutorial::Update(float seconds_elapsed, bool &cameraLocked) {
	Game* g = Game::instance;
	SDL_ShowCursor(false); //NO mostrem el cursor
	cameraLocked = true;

	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE)) {  // TECLA ESC
		g->SetStage(MENU);
		return; //acabar el update
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) { // TECLA TAB
		g->SetStage(EDITMODE);
		return; //acaba el update
	}
	slowMotion = true;
	float playerSpeed = 2.5f * g->elapsed_time;
	float rotSpeed = 120.0f * g->elapsed_time;

	Input::centerMouse();
	
	player->pitch += -Input::mouse_delta.y * 10.0f * g->elapsed_time;

	if (player->pitch < player->max_pitch.x) {
		player->pitch = -40.0f;
	}
	else if (player->pitch > player->max_pitch.y) {
		player->pitch = 40.0f;
	}

	player->yaw += -Input::mouse_delta.x * 10.0f * g->elapsed_time;

	Matrix44 playerRotation;
	playerRotation.rotate(player->yaw * DEG2RAD, Vector3(0, 1, 0));

	Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
	Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
	Vector3 playerVel;


	if (Input::isKeyPressed(SDL_SCANCODE_W)) { playerVel = playerVel + (playerSpeed * forward); slowMotion = false; }
	if (Input::isKeyPressed(SDL_SCANCODE_S)) { playerVel = playerVel - (playerSpeed * forward);  slowMotion = false; }
	if (Input::isKeyPressed(SDL_SCANCODE_D)) { playerVel = playerVel + (playerSpeed * right); slowMotion = false; }
	if (Input::isKeyPressed(SDL_SCANCODE_A)) { playerVel = playerVel - (playerSpeed * right);  slowMotion = false; }

	if (slowMotion) {
		g->elapsed_time *= 0.1f;

	}
	//Colisions dels Players (player + enemcis)
	player->checkColisions(playerVel, world.entities, g->elapsed_time, 0.2f);
	

	//update bala de la posicio i si colisiona amb enemics o parets
	world.shooting_update(player_entity);
	//AI ENEMIES - Canvi de posicio dels enemics  + comprovar colisions enemics
	for (size_t i = 0; i < world.player_enemies.size(); i++) {
		Player* enemy = world.player_enemies[i];
		enemy->AIEnemy(g->elapsed_time, player, world.entities, world.enemies, world.bullets, cameraLocked);
	}
}

void Tutorial::renderSkyGround(Camera* camera, bool cameraLocked){
	Game* g = Game::instance;
	Matrix44 skyModel;
	skyModel.translate(camera->eye.x, camera->eye.y - 40.0f, camera->eye.z);
	Entity background = Entity(skyModel, g->mesh_sky, g->texture_sky);
	glDisable(GL_DEPTH_TEST); //desactivem el DEPTH TEST abans de redenritzar el fons perque no es superposi devant entitats
	background.RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
	glEnable(GL_DEPTH_TEST);
	Matrix44 groundModel;
	Entity ground = Entity(groundModel, g->mesh_ground, g->texture_ground);
	ground.RenderEntity(GL_TRIANGLES, g->shader, camera, cameraLocked);
}
void Tutorial::onKeyDown(SDL_KeyboardEvent event) {

}

Level::Level() {

}
void Level::Render(bool cameraLocked) {

}
void Level::Update(float seconds_elapsed, bool &cameraLocked) {

}
void Level::onKeyDown(SDL_KeyboardEvent event) {

}

Final::Final() {

}
void Final::Render(bool cameraLocked) {

}
void Final::Update(float seconds_elapsed, bool &cameraLocked) {

}
void Final::onKeyDown(SDL_KeyboardEvent event) {

}

EditMode::EditMode() {
	Game* g = Game::instance;
	g->mouse_speed = 100.0f;
	//pathfinding
	world.creteGrid();

}
void EditMode::Render(bool cameraLocked) {
	Game* g = Game::instance;

	std::string text_edicio = "F1 Reload All\n 0 Save World\n 2 Add Entity\n 3 Select Entity\n 4 Rotate <-\n 5 Rotate ->\n 6 Remove Entity\n 9 Load World\n + Change Entity to Add\n";
	drawText(g->window_width - 200, 2, text_edicio, Vector3(1, 1, 1), 2);
	//Draw the floor grid
	drawGrid();

	//Pathfinding
	//world.renderPath(cameraLocked);

}
void EditMode::Update(float seconds_elapsed, bool &cameraLocked) {
	Game* g = Game::instance;
	Camera* camera = g->camera;
	cameraLocked = false;
	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		g->SetStage(TUTORIAL);
		return; //acaba el update
	}
	float speed = seconds_elapsed * g->mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || g->mouse_locked)
	{
		g->camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		g->camera->rotate(Input::mouse_delta.y * 0.005f, Vector3(-1.0f, 0.0f, 0.0f));
	}
	SDL_ShowCursor(true); //mostrem el cursor
	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_E)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
}
void EditMode::onKeyDown(SDL_KeyboardEvent event) {
	Game* g = Game::instance;

	switch (event.keysym.sym)
	{
		case SDLK_ESCAPE: {
			g->GetStage(MENU);
			return;
		}
	case SDLK_F1: Shader::ReloadAll(); break;
	case SDLK_1: world.AddEntityInFront(g->camera, Entity::ENTITY_ID::ENEMY); break; //afegir enemic
	case SDLK_2: world.AddEntityInFront(g->camera, (Entity::ENTITY_ID)entityToAdd); break; //afegir entitats estatiques
	case SDLK_3: selectedEntity = world.RayPick(g->camera, selectedEntity);
		if (selectedEntity == NULL) printf("selected entity not saved!\n");
		break;
	case SDLK_4:  world.RotateSelected(10.0f, selectedEntity); break;
	case SDLK_5:  world.RotateSelected(-10.0f, selectedEntity); break;
	case SDLK_6:  world.DeleteEntity(g->camera); break;
	case SDLK_0: world.saveWorld(); break;
		//path finding
	case SDLK_7: {
		Vector2 mouse = Input::mouse_position;
		Game* g = Game::instance;
		Vector3 dir = g->camera->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
		Vector3 rayOrigin = g->camera->eye;

		Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
		world.start_x = clamp(spawnPos.x / world.tileSizeX, 0, world.W);
		world.start_y = clamp(spawnPos.z / world.tileSizeY, 0, world.H);
		//printf("(start_x, start_y) = (%d, %d)\n", start_x, start_y);
		break;
	}
	case SDLK_8: {
		Vector2 mouse = Input::mouse_position;
		Game* g = Game::instance;
		Vector3 dir = g->camera->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
		Vector3 rayOrigin = g->camera->eye;

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
	case SDLK_9: world.loadWorld(); break;
	case SDLK_PLUS: entityToAdd = (entityToAdd + 1) % 5; //canviar enum sense bullet (enum = 5) i el 6 es el enemic

	}
}




Menu::Menu() {
	wasLeftMousePressed = false;
}
void Menu::Render(bool cameraLocked) {
	Game* g = Game::instance;
	//Render All GUI ----------------------------------
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	if (RenderButton(g->window_width / 2, 100, 600, 100, g->play)) {
		g->SetStage(TUTORIAL);
		printf("Play\n");
	}
	else if (RenderButton(g->window_width / 2, 200, 600, 100, g->restart)) {
		world.restartWorld();
		g->SetStage(TUTORIAL);
		printf("Restart\n");
	}
	else if (RenderButton(g->window_width / 2, 300, 600, 100, g->save)) {
		printf("Save\n");
	}
	else if (RenderButton(g->window_width / 2, 400, 600, 100, g->exit)) {
		printf("Exit\n");
		g->must_exit = true;
	}
	
	wasLeftMousePressed = false;
}
void Menu::Update(float seconds_elapsed, bool &cameraLocked) {
	Game* g = Game::instance;
	cameraLocked = true;

	SDL_ShowCursor(true);
	if (Input::wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
		g->SetStage(TUTORIAL);
		return; //acaba el update
	}
}
void Menu::onKeyDown(SDL_KeyboardEvent event) {

}

bool Menu::RenderButton(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV) {
	Vector2 mouse = Input::mouse_position;
	float halfWidth = w * 0.5;
	float halfHeight = h * 0.5;
	float min_x = x - halfWidth;
	float max_x = x + halfWidth;
	float min_y = y - halfHeight;
	float max_y = y + halfHeight;

	bool hover = mouse.x >= min_x && mouse.x <= max_x && mouse.y >= min_y && mouse.y <= max_y;
	Vector4 buttonColor = hover ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.7f);

	RenderGUI(x, y, w, h, texture, buttonColor, flipYV);
	return wasLeftMousePressed && hover;
}

void Menu::RenderGUI(float x, float y, float w, float h, Texture* texture, Vector4 color, bool flipYV) {
	Game* g = Game::instance;
	int window_width = g->window_width;
	int window_height = g->window_height;
	Mesh quad;
	quad.createQuad(x, y, w, h, flipYV);

	Camera cam2D;
	cam2D.setOrthographic(0, window_width, window_height, 0, -1, 1);

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	//Texture* texture = Texture::Get("data/gui/play-button.png");

	if (!shader) return;
	shader->enable();

	shader->setUniform("u_color", color);
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	if (texture != NULL) {
		shader->setUniform("u_texture", texture, 0);
	}
	shader->setUniform("u_time", g->time);
	//shader->setUniform("u_tex_tiling", 1.0f);
	shader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);

	shader->disable();
}


