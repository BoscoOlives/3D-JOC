#ifndef ENTITY_H
#define ENTITY_H
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "camera.h"
#include "shader.h"
#include "includes.h"
#include "animation.h"



class Entity {
public:
	static Entity* instance;
	Matrix44 model;
	Mesh* mesh;
	Texture* texture;
	Vector3 dir;
	float yaw;
	std::vector<Animation*> animations;

	enum ENTITY_ID {
		BARREL = 0,
		CONSOLE = 1,
		SUPPORT = 2,
		ROCK1 = 3,
		BULLET = 4,
		ENEMY = 5
	};

	ENTITY_ID current_entity;
	

	Entity(Matrix44 model, Mesh* mesh, Texture* texture);
	void RenderEntity(int primitive, Shader* a_shader, Camera* cam, bool cameraLocked, float tiling = 1.0f);
	void RenderEntityAnim(int primitive, Shader* a_shader, Camera* cam, Vector3 pos, float yaw, bool look, bool slowMotion, bool cameraLocked);
	void update_position_moving(float elapsed_time, float vel);
	
};


#endif
