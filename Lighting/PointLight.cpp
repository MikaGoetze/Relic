#include "PointLight.h"
#include <iostream>

PointLight::PointLight(glm::vec3 pos, glm::vec3 color, float intensity, Shader* shader) : Light("pointLights[" + std::to_string(NUM_P_LIGHTS) + "]", color, intensity, shader)
{
	index = NUM_P_LIGHTS;
	NUM_P_LIGHTS++;
	position = pos;

	PointLight::Initialise();
}

PointLight::~PointLight()
{
}

void PointLight::Initialise()
{
	Light::Initialise();
	SHADER->SetVec3(lightLocation + ".position", position);
	SHADER->SetInt("num_p_lights", NUM_P_LIGHTS);
}


void PointLight::SetPosition(glm::vec3 pos)
{
	position = pos;
	SHADER->SetVec3(lightLocation + ".position", position);
}

glm::vec3 PointLight::GetPosition()
{
	return position;
}
