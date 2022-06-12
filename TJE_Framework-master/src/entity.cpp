#include "entity.h"
#include "input.h"
#include "Game.h"

Entity* Entity::instance = NULL;

Entity::Entity(Matrix44 model, Mesh* mesh, Texture* texture) {
	instance = this;
	this->model = model;
	this->mesh= mesh;
	this->texture = texture;

}


void Entity::RenderEntity(int primitive, Shader* a_shader, Camera* cam, bool cameraLocked, float tiling) {
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
    a_shader->setUniform("u_tex_tiling", tiling);
	a_shader->setUniform("u_model", model);
	mesh->render(primitive);

	a_shader->disable();
    
    if (!cameraLocked) {
        mesh->renderBounding(model);
    }

}

void Entity::update_position_moving(float elapsed_time, float vel) {
    Vector3 pos;
    pos = this->model.getTranslation();
	
    pos = pos + this->dir * vel * elapsed_time;

    this->model.setTranslation(pos.x, pos.y, pos.z);
	this->model.rotate(Game::instance->player->yaw * DEG2RAD, Vector3(0, 1, 0));
}

void Entity::RenderEntityAnim(int primitive, Shader* a_shader, Camera* cam, Vector3 pos, float yaw, bool look) {
	assert((mesh != NULL, "mesh in renderMesh was null"));
	if (!a_shader) return;

	Game* g = Game::instance;
	float time = g->time;
	Animation* idle = g->anim_idle;
	Animation* run = g->anim_run;

	
	float t = fmod(time, run->duration) / run->duration;
	idle->assignTime(time);
	run->assignTime(t* run->duration);
	if (g->slowMotion) {

		float t = fmod(time, run->duration*2.0f) / run->duration*0.5f;
		idle->assignTime(time*0.5);
		run->assignTime(t * run->duration);
	}

	Skeleton resultSk;

	if (look) {
		blendSkeleton(&idle->skeleton, &run->skeleton, 1.0f, &resultSk);
	}
	else{
		blendSkeleton(&idle->skeleton, &run->skeleton, 0.0f, &resultSk);
	}

	//actualitzem la model amb els valors del player de POS i YAW
	model.setTranslation(pos.x, pos.y, pos.z);
	model.rotate(yaw * DEG2RAD, Vector3(0, 1, 0));
	model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));

	model.scale(0.01, 0.01, 0.01); //els enemics tenen un escala gigant i s'ha de reescalar cada cop

	mesh->createCollisionModel(false);
	//enable shader upload uniforms
	a_shader->enable();
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	if (texture != NULL) {
		a_shader->setUniform("u_texture", texture, 0);
	}
	a_shader->setUniform("u_time", time);
	a_shader->setUniform("u_tex_tiling", 1.0f);
	a_shader->setUniform("u_model", model);
	mesh->renderAnimated(primitive, &resultSk);

	a_shader->disable();

	//Render de l'arma
	if(look)
	{
		Matrix44 neckLocalMatrix = resultSk.getBoneMatrix("mixamorig_RightHand", false);
		neckLocalMatrix.rotate(90.0f * DEG2RAD, Vector3(0.0, -1.0, 1.0));

		Matrix44 localToWorldMatrix = neckLocalMatrix * model;

		localToWorldMatrix.translate(-13.0f, 9.0f, 6.0f); //es una tirita, no mola fer-ho així

		Entity* pistol_entity = new Entity(localToWorldMatrix, g->mesh_pistol_e, g->texture_black);
		pistol_entity->RenderEntity(GL_TRIANGLES, a_shader, cam, g->cameraLocked);
	}

	if (!g->cameraLocked) {
		mesh->renderBounding(model);
	}

}

