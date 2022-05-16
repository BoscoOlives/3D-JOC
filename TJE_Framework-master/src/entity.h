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
	void RenderEntity(int primitive, Shader* a_shader, Camera* cam, bool cameraLocked, float tiling = 1.0f);
	
};

#endif
