#ifndef P_LIGHT_H
#define P_LIGHT_H

#include "Light.h"
class PointLight : public Light
{
public:
	void Update() override;
	void Serialize() override;
	void Deserialize() override;
	PointLight(glm::vec3 color, float intensity, Shader* shader);
	unsigned int GetIndex()
	{
		return index;
	}
	~PointLight();
	void Initialise();
private:
	unsigned int index;
};

#endif