#ifndef DIR_LIGHT_H
#define DIR_LIGHT_H
#include "Light.h"
class DirectionalLight : public Light
{
public:
	void Update() override;
	void Serialize() override;
	void Deserialize() override;
	DirectionalLight(glm::vec3 dir, glm::vec3 color, float intensity, Shader* shader);
	~DirectionalLight();
	void Initialise() override;

	void SetDirection(glm::vec3 dir);
	glm::vec3 GetDirection();
private:
	glm::vec3 direction;
};

#endif