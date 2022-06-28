#include "bullet.h"
#include "game.h"


Bullet* Bullet::instance = NULL;

Bullet::Bullet(Matrix44 model, Mesh* mesh, Texture* texture) {
	instance = this;
	this->mesh = mesh;
	this->tex = texture;
	this->model = model;
	isActive = false;
	this->author = 0;
	entity_bullet = new Entity(this->model, this->mesh, this->tex);
	entity_bullet->current_entity = Entity::ENTITY_ID::BULLET;
}