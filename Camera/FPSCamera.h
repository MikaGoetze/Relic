#ifndef FPS_CAM_H
#define FPS_CAM_H
#include "Camera.h"
#include <GLFW/glfw3.h> 
#include <glm/gtc/type_ptr.hpp>

class FPSCamera
{
public:
	FPSCamera(bool mouse_controls = true);

	void SetSensitivity(float sens);
	void Rotate(float yaw, float pitch);
	const glm::mat4& GetView();
	const glm::vec3& GetPosition();
	void SetPosition(glm::vec3 position);
	const glm::vec3& GetDirection();
	const glm::vec3& GetUp();
	const glm::vec3& GetRight();

private:
	static FPSCamera* instance;
	static void process_mouse_input_static(GLFWwindow* window, double xpos, double ypos);
	~FPSCamera();
	void process_mouse_input(GLFWwindow* window, double xpos, double ypos);

	float lastMouseX;
	float lastMouseY;

	float yaw;
	float pitch;

	bool firstMouse;
	float sensitivity;

	Camera* cam;
};

#endif