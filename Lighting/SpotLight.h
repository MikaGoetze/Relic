#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H
#include "Light.h"
class SpotLight : public Light
{
public:
	void Start() override;
	void Update() override;
	void Serialize() override;
	void Deserialize() override;
	SpotLight(glm::vec3 dir, glm::vec3 color, float cutOff, float outerCutOff, float intensity, Shader* shader);
	~SpotLight();
	void Initialise();

	void SetDirection(glm::vec3 dir);
	glm::vec3 GetDirection();
private:
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

};

#endif