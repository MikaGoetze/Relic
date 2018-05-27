#ifndef CAMERA_H
#define CAMERA_H
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	void RotateAround(glm::vec3 rot, glm::vec3 target);
	void SetPosition(glm::vec3 position);
	void SetDirection(glm::vec3 direction);
	void SetDirectionEuler(glm::vec2 euler);
	const glm::mat4& GetView();
	const glm::vec3& GetPosition();
	const glm::vec3& GetDirection();
	const glm::vec3& GetUp();
	const glm::vec3& GetRight();

	void SetView(glm::mat4 view);

private:
	glm::vec3 cameraPos;
	glm::vec3 up;
	glm::vec3 right;
	glm::mat4 view;
	glm::vec3 cameraFront;
};


#endif