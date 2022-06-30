#include "light.h"



Light::Light()
{
	//Illumination Elements
	position.set(15, 8, 9);
	sun_color.set(0.560, 0.560, 0.560);
	//specular_color.set(0.31, 0.31, 0.31);

	//Material Elements
	ambient.set(0.8, 0.8, 0.8); //reflected ambient light
	//diff_factor.set(0.905, 0.866, 0.815); //reflected diffuse light
	diff_factor.set(0.3, 0.3, 0.3); //reflected diffuse light
	spec_factor.set(0.4, 0.4, 0.4); //reflected specular light
	glossiness = 10.0; //glosiness coefficient (plasticity)
	//https://doc.instantreality.org/tools/color_calculator/
	ambient_light = { 0.866, 0.784, 0.674 }; //here we can store the global ambient light of the scene
}

void Light::calcKaia() {
	//CALCULATE AMBIENT LIGHT
	kaia = { ambient.x * ambient_light.x, ambient.y * ambient_light.y, ambient.z * ambient_light.z };

}

void Light::illumination(Shader* a_shader, Camera* cam) {
	//printf("X, Y, Z, %f, %f, %f\n", cam->eye.x, cam->eye.y, cam->eye.z);
	a_shader->setVector3("ambient", kaia);
	a_shader->setVector3("light_pos", position);
	a_shader->setVector3("sun_color", sun_color);
	a_shader->setVector3("diff_factor", diff_factor);
	a_shader->setVector3("spec_factor", spec_factor);

	a_shader->setFloat("glossiness", glossiness);
	a_shader->setVector3("camera_eye", cam->eye );

}


