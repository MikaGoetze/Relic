#ifndef P_LIGHT_H
#define P_LIGHT_H

#include "Light.h"
class PointLight : public Light
{
public:
	PointLight(glm::vec3 pos, glm::vec3 color, float intensity, Shader* shader);
	~PointLight();
	void Initialise();
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetPosition();
private:
	unsigned int index;
	glm::vec3 position;
};

#endif