#pragma once

#include "utils.h"
#include "camera.h"
#include "shader.h"


//This class contains all the info about the properties of the light
class Light
{
public:
	Vector3 position; //where is the light
	Vector3 sun_color; //the amount (and color) of diffuse
	//Vector3 specular_color; //the amount (and color) of specular

	
	Vector3 diff_factor; //reflected diffuse light
	Vector3 spec_factor; //reflected specular light
	float glossiness; //glosiness coefficient (plasticity)
	
	Vector3 ambient; //reflected ambient light
	Vector3 ambient_light;
	Vector3 kaia;

	Light();
	void calcKaia();
	void illumination(Shader* a_shader, Camera* cam);


};

