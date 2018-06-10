#ifndef P_LIGHT_H
#define P_LIGHT_H

#include "Light.h"
class PointLight : public Light
{
public:
	void Start() override;
	void Update() override;
	void Serialize() override;
	void Deserialize() override;
	PointLight(glm::vec3 color, float intensity, Shader* shader);
	void SetFar(float far);
	float GetFar();
	unsigned int GetIndex()
	{
		return index;
	}
	~PointLight();
	void Initialise();
private:
	unsigned int index;
	float far;
};

#endif