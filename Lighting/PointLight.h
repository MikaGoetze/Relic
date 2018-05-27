#ifndef P_LIGHT_H
#define P_LIGHT_H

#include "Light.h"
class PointLight : public Light
{
public:
	PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cons, float lin, float quad, Shader* shader);
	PointLight(glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float range, Shader* shader);
	~PointLight();
	void Initialise();
	void SetProperties(float cons, float lin, float quad);
	void SetRange(float range);
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();
private:
	unsigned int index;
	glm::vec3 position;
	float cons;
	float lin;
	float quad;

	void SetFalloffFactors(float range);
};

#endif