#ifndef CAMERA_H
#define CAMERA_H
#include <glm/gtc/type_ptr.hpp>
#include "Core/RelicBehaviour.h"
#include <Core/Transform.h>

class Camera : public RelicBehaviour
{
public:
	void Serialize() override;
	void Deserialize() override;
	Camera();
	~Camera();

	void Start() override;

	void RotateAround(glm::vec3 rot, glm::vec3 target);
	void SetDirection(glm::vec3 direction);
	void SetDirectionEuler(glm::vec2 euler);
	const glm::mat4& GetView();
	const glm::vec3& GetDirection();
	const glm::vec3& GetUp();
	const glm::vec3& GetRight();

	void SetView(glm::mat4 view);

	static Camera* main;

private:
	Transform * transform = NULL;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 view;
	glm::vec3 cameraFront;
};


#endif