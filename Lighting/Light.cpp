#include "Light.h"
#include <iostream>

void Light::Serialize()
{
}

void Light::Deserialize()
{
}

Light::Light(std::string lightLoc, glm::vec3 color, float intensity, Shader* shader)
{
	if (shader != NULL && SHADER == NULL) {
		SHADER = shader;
	}

	this->color = color;
	this->intensity = intensity;
	this->depth_map = 0;
	this->light_space = glm::mat4();
	lightLocation = lightLoc;
}

void Light::SetDepthMap(unsigned map)
{
	depth_map = map;
	SHADER->SetInt(lightLocation + ".depth_map", depth_map);
}

void Light::SetLightSpace(glm::mat4 space)
{
	light_space = space;
}

Light::~Light()
{
}

void Light::Initialise()
{
	SHADER->SetVec3(lightLocation + ".color", color);
	SHADER->SetInt(lightLocation + ".depth_map", depth_map);
	SHADER->SetMat4(lightLocation + ".light_space", light_space);
	SHADER->SetFloat(lightLocation + ".intensity", intensity);
}

void Light::SetActive(bool active)
{
	SHADER->SetBool(lightLocation + ".enabled", active);
}


Shader* Light::SHADER;
unsigned int Light::NUM_D_LIGHTS;
unsigned int Light::NUM_P_LIGHTS;
unsigned int Light::NUM_S_LIGHTS;


