#ifndef bullet_h
#define bullet_h
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "entity.h"

class Bullet {
public:
	static Bullet* instance;
	Matrix44 model;
	Mesh* mesh;
	Texture* tex;
	Entity* entity_bullet = NULL;
	int author;
	bool isActive;
	Bullet(Matrix44 model, Mesh* mesh, Texture* texture); //constructor

};
#endif