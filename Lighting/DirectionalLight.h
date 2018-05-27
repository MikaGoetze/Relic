#ifndef DIR_LIGHT_H
#define DIR_LIGHT_H
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, Shader* shader);
	~DirectionalLight();
	void Initialise() override;

	void SetDirection(glm::vec3 dir);
	glm::vec3 GetDirection();
private:
	glm::vec3 direction;
};

#endif