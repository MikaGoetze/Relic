#include "PointLight.h"
#include <iostream>

PointLight::PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cons, float lin, float quad, Shader* shader) : Light("pointLights[" + std::to_string(NUM_P_LIGHTS) + "]", amb, diff, spec, shader)
{
	index = NUM_P_LIGHTS;
	NUM_P_LIGHTS++;
	position = pos;

	this->cons = cons;
	this->lin = lin;
	this->quad = lin;
	PointLight::Initialise();
}

PointLight::~PointLight()
{
}

void PointLight::Initialise()
{
	Light::Initialise();
	SHADER->SetVec3(lightLocation + ".position", position);
	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
	SHADER->SetInt("num_p_lights", NUM_P_LIGHTS);
}

void PointLight::SetProperties(float cons, float lin, float quad)
{
	this->cons = cons;
	this->lin = lin;
	this->quad = quad;

	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
}

void PointLight::SetRange(float range)
{
	SetFalloffFactors(range);
	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
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

void PointLight::SetFalloffFactors(float range)
{
	cons = 1;
	lin =  2 / range;
	quad = 1/(range * range);
}