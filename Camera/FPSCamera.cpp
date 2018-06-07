#include <Core/Relic.h>
#include "FPSCamera.h"


FPSCamera* FPSCamera::instance;

void FPSCamera::Start()
{
	cam = GetGameObject()->GetComponent<Camera>();
	sensitivity = 0.5f;
	lastMouseX = 0;
	lastMouseY = 0;

	yaw = 90;
	pitch = 0;

	firstMouse = true;
}

void FPSCamera::Serialize()
{
}

void FPSCamera::Deserialize()
{
}

FPSCamera::FPSCamera(bool mouse_controls)
{
	if (instance != NULL)
	{
		delete instance;
	}
	instance = this;
	
	if (mouse_controls) {
		glfwSetCursorPosCallback(Relic::GetWindow()->InternalWindow(), process_mouse_input_static);
		glfwSetInputMode(Relic::GetWindow()->InternalWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

FPSCamera::~FPSCamera()
{
	delete cam;
}

void FPSCamera::process_mouse_input_static(GLFWwindow* window, double xpos, double ypos)
{
	instance->process_mouse_input(window, xpos, ypos);
}


void FPSCamera::process_mouse_input(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastMouseX = xpos;
		lastMouseY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastMouseX;
	float yOffset = lastMouseY - ypos;

	lastMouseX = xpos;
	lastMouseY = ypos;

	float sensitivity = 0.05f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89) pitch = 89;
	if (pitch < -89) pitch = -89;

	cam->SetDirectionEuler(glm::vec2(yaw, pitch));
}

void FPSCamera::SetSensitivity(float sens)
{
	sensitivity = sens;
}

void FPSCamera::Rotate(float y, float p)
{
	this->yaw += y;
	this->pitch += p;

	if (pitch > 89) pitch = 89;
	if (pitch < -89) pitch = -89;

	cam->SetDirectionEuler(glm::vec2(yaw, pitch));
}

const glm::mat4& FPSCamera::GetView()
{
	return cam->GetView();
}

const glm::vec3& FPSCamera::GetDirection()
{
	return cam->GetDirection();
}

const glm::vec3& FPSCamera::GetUp()
{
	return cam->GetUp();
}

const glm::vec3& FPSCamera::GetRight()
{
	return cam->GetRight();
}
