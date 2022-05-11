#include "entity.h"
#include "input.h"
#include "Game.h"

Entity* Entity::instance = NULL;

Entity::Entity(Matrix44 model, Mesh* mesh, Texture* texture) {
	instance = this;
	this->model = model;
	this->mesh = mesh;
	this->texture = texture;

}


void Entity::RenderEntity(int primitive, Shader* a_shader, Camera* cam) {
	assert((mesh != NULL, "mesh in renderMesh was null"));
	if (!a_shader) return;

	float time = Game::instance->time;
	//enable shader
	
	a_shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	if (texture != NULL) {
		a_shader->setUniform("u_texture", texture, 0);
	}
	a_shader->setUniform("u_time", time);
	a_shader->setUniform("u_model", model);
	mesh->render(primitive);

	a_shader->disable();
}


std::vector<Entity*> AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture, std::vector<Entity*> entities) {
	Vector2 mousePos = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;


	Vector3 spawnPos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);


	Entity* entity = new Entity(model, mesh, texture);
	entities.push_back(entity);

	return entities;
}

std::vector<Vector3> RayPickCheck(Camera* cam, std::vector<Vector3> points, std::vector<Entity*> entities) {
	Vector2 mousePos = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mousePos.x, mousePos.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < entities.size(); i++)
	{
		Entity* entity = entities[i];
		Vector3 pos;
		Vector3 normal;
		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
			points.push_back(pos);
		}
	}
	return points;
}

