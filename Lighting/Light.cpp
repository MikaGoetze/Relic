#include "Light.h"
#include <iostream>

Light::Light(std::string lightLoc, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, Shader* shader)
{
	if (shader != NULL && SHADER == NULL) {
		SHADER = shader;
	}
	if (SHADER == NULL)
	{
		SHADER = new Shader("lighting.vert", "lighting.frag");
	}

	this->amb = amb;
	this->spec = spec;
	this->diff = diff;
	lightLocation = lightLoc;
}

Light::~Light()
{
}

void Light::Initialise()
{
	SHADER->SetVec3(lightLocation + ".ambient", amb);
	SHADER->SetVec3(lightLocation + ".diffuse", diff);
	SHADER->SetVec3(lightLocation + ".specular", spec);
}

void Light::SetActive(bool active)
{
	SHADER->SetBool(lightLocation + ".enabled", active);
}


Shader* Light::SHADER;
unsigned int Light::NUM_D_LIGHTS;
unsigned int Light::NUM_P_LIGHTS;
unsigned int Light::NUM_S_LIGHTS;


