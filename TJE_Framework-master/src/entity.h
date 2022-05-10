#ifndef ENTITY_H
#define ENTITY_H
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
#include "includes.h"


class Entity {
public:
	static Entity* instance;
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;

	Entity(Matrix44 model, Mesh* mesh, Texture* texture);
	void RenderEntity(Shader* a_shader, Camera* cam);
	
};

std::vector<Entity*> AddEntityInFront(Camera* cam, Mesh* mesh, Texture* texture, std::vector<Entity*> entities);

#endif