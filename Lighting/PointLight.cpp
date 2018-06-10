#include "PointLight.h"
#include <iostream>
#include <Core/Transform.h>

PointLight::PointLight(glm::vec3 color, float intensity, Shader* shader) : Light("pointLights[" + std::to_string(NUM_P_LIGHTS) + "]", color, intensity, shader)
{
	index = NUM_P_LIGHTS;
	NUM_P_LIGHTS++;

	this->far = 40.0f;
}

void PointLight::SetFar(float far)
{
	this->far = far;
	SHADER->SetFloat(lightLocation + ".far", far);
}

float PointLight::GetFar()
{
	return far;
}

void PointLight::Start()
{
	PointLight::Initialise();
}

void PointLight::Update()
{
	Initialise();
}

void PointLight::Serialize()
{
}

void PointLight::Deserialize()
{
}

PointLight::~PointLight()
{
}

void PointLight::Initialise()
{
	Light::Initialise();
	SHADER->SetInt("num_p_lights", NUM_P_LIGHTS);
	SHADER->SetFloat(lightLocation + ".far", far);
	SHADER->SetVec3(lightLocation + ".position", GetGameObject()->GetComponent<Transform>()->GetPosition());
}
