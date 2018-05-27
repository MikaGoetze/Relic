#include "SpotLight.h"

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cutOff,
	float outerCutOff, float cons, float lin, float quad, Shader* shader) : Light("spotLight", amb, diff, spec, shader)
{
	position = pos;
	direction = dir;
	
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;
	this->cons = cons;
	this->lin = lin;
	this->quad = quad;

	SpotLight::Initialise();

	NUM_S_LIGHTS++;
}

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cutOff,
	float outerCutOff, float range, Shader* shader) : Light("spotLight", amb, diff, spec, shader)
{
	position = pos;
	direction = dir;
	
	this->cutOff = cutOff;
	this->outerCutOff = outerCutOff;

	SetFalloffFactors(range);

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
	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
}

void SpotLight::SetProperties(float cons, float lin, float quad)
{
	this->cons = cons;
	this->lin = lin;
	this->quad = quad;

	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
}

void SpotLight::SetRange(float range)
{
	SetFalloffFactors(range);
	SHADER->SetFloat(lightLocation + ".constant", cons);
	SHADER->SetFloat(lightLocation + ".linear", lin);
	SHADER->SetFloat(lightLocation + ".quadratic", quad);
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

void SpotLight::SetFalloffFactors(float range)
{
	cons = 1;
	lin =  2 / range;
	quad = 1/(range * range);
}
