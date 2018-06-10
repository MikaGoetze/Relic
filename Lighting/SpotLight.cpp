#include "SpotLight.h"
#include <Core/GameObject.h>
#include <Core/Transform.h>

void SpotLight::Start()
{
	SpotLight::Initialise();
}

void SpotLight::Update()
{
	Initialise();
}

void SpotLight::Serialize()
{
}

void SpotLight::Deserialize()
{
}

SpotLight::SpotLight(glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, float intensity, Shader* shader) : Light("spotLight", color, intensity, shader)
{
	direction = dir;
	
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;

	NUM_S_LIGHTS++;
}

SpotLight::~SpotLight()
{
}

void SpotLight::Initialise()
{
	Light::Initialise();
	SHADER->SetVec3(lightLocation + ".position", GetGameObject()->GetComponent<Transform>()->GetPosition());
	SHADER->SetVec3(lightLocation + ".direction", direction);
	SHADER->SetFloat(lightLocation + ".cutOff", glm::cos(glm::radians(cutOff)));
	SHADER->SetFloat(lightLocation + ".outerCutOff", glm::cos(glm::radians(outerCutOff)));

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

