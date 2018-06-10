#include "DirectionalLight.h"
#include <iostream>

void DirectionalLight::Update()
{
	Initialise();
}

void DirectionalLight::Serialize()
{
}

void DirectionalLight::Deserialize()
{
}

DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 color, float intensity, Shader* shader) : Light("sun", color, intensity, shader)
{
	direction = dir;
	DirectionalLight::Initialise();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Initialise()
{
	Light::Initialise();
	SHADER->SetVec3(lightLocation + ".direction", direction);
}

void DirectionalLight::SetDirection(glm::vec3 dir)
{
	direction = dir;
//	std::cout << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
	SHADER->SetVec3(lightLocation + ".direction", direction);
}

glm::vec3 DirectionalLight::GetDirection()
{
	return direction;
}
