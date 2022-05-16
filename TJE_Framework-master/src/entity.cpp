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
