#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, float intensity, Shader* shader) : Light("spotLight", color, intensity, shader)
{
	position = pos;
	direction = dir;
	
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
	SpotLight::Initialise();

	NUM_S_LIGHTS++;
}

SpotLight::~SpotLight()
{
}

void SpotLight::Initialise()
{
	Light::Initialise();
	SHADER->SetVec3(lightLocation + ".position", position);
	SHADER->SetVec3(lightLocation + ".direction", direction);
	SHADER->SetFloat(lightLocation + ".cutOff", glm::cos(glm::radians(cutOff)));
	SHADER->SetFloat(lightLocation + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));

}

void SpotLight::SetPosition(glm::vec3 pos)
{
	position = pos;
	SHADER->SetVec3(lightLocation + ".position", position);
}

glm::vec3 SpotLight::GetPosition()
{
	return position;
}

void SpotLight::SetDirection(glm::vec3 dir)
{
	direction = dir;
	SHADER->SetVec3(lightLocation + ".direction", direction);
}

glm::vec3 SpotLight::GetDirection()
{
	return direction;
}

