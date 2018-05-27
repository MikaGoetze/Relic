#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H
#include "Light.h"
class SpotLight : public Light
{
public:
	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cutOff, float outerCutOff, float cons, float lin, float quad, Shader* shader);
	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, float cutOff, float outerCutOff, float range, Shader* shader);
	~SpotLight();
	void Initialise();
	void SetProperties(float cons, float lin, float quad);
	void SetRange(float range);
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();

	void SetDirection(glm::vec3 dir);
	glm::vec3 GetDirection();
private:
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;
	float cons;
	float lin;
	float quad;

	void SetFalloffFactors(float range);

};

#endif