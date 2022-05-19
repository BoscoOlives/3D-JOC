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

	enum ENTITY_ID {
		HOUSE = 0,
		BULLET = 1,
	};

	ENTITY_ID current_entity;
	

	Entity(Matrix44 model, Mesh* mesh, Texture* texture);
	void RenderEntity(int primitive, Shader* a_shader, Camera* cam, bool cameraLocked, float tiling = 1.0f);
	
};

class Bullet : public Entity {
public:
	Bullet(Matrix44 model, Mesh* mesh, Texture* texture, Vector3 pos);
	Vector3 pos;
	int vel;
};

#endif
