#include "Light.h"
#include <iostream>

Light::Light(std::string lightLoc, glm::vec3 color, float intensity, Shader* shader)
{
	if (shader != NULL && SHADER == NULL) {
		SHADER = shader;
	}
	if (SHADER == NULL)
	{
		SHADER = new Shader("lighting.vert", "lighting.frag");
	}

	this->color = color;
	this->intensity = intensity;
	lightLocation = lightLoc;
}

Light::~Light()
{
}

void Light::Initialise()
{
	SHADER->SetVec3(lightLocation + ".color", color);
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


