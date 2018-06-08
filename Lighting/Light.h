#ifndef LIGHT_H
#define LIGHT_H
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Light
{
public:
;
	Light(std::string lightLoc, glm::vec3 color, float intensity, Shader* shader = NULL);
	virtual void Initialise();

	void SetActive(bool active);

	static Shader* SHADER;
	static unsigned int NUM_P_LIGHTS;
	static unsigned int NUM_D_LIGHTS;
	static unsigned int NUM_S_LIGHTS;
	const static int MAX_P_LIGHTS = 4;
protected:
	std::string lightLocation;
	float intensity;
	glm::vec3 color;

	virtual ~Light();

};

#endif