#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H
#include "Light.h"
class SpotLight : public Light
{
public:
	SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, float intensity, Shader* shader);
	~SpotLight();
	void Initialise();
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();

	void SetDirection(glm::vec3 dir);
	glm::vec3 GetDirection();
private:
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

};

#endif