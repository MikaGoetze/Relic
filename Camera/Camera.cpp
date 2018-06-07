#include "Camera.h"
#include <glm/gtc/matrix_transform.inl>


void Camera::Serialize()
{
}

void Camera::Deserialize()
{
}

Camera::Camera()
{
	//Lets just initiallise this stuff to start with

	if(main == NULL)
	{
		main = this;
	}
}

Camera* Camera::main;

Camera::~Camera()
{
}

void Camera::Start()
{
	transform = GetGameObject()->GetComponent<Transform>();
	glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDir = glm::normalize(transform->GetPosition() - target);
	
	right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), cameraDir));
	up = glm::cross(cameraDir, right);

	cameraFront = glm::vec3(0, 0, 1);
}


void Camera::RotateAround(glm::vec3 rot, glm::vec3 target)
{
	view = glm::lookAt(rot, target, glm::vec3(0,1,0));
}


void Camera::SetDirection(glm::vec3 direction)
{
	cameraFront = direction;
}

void Camera::SetDirectionEuler(glm::vec2 euler)
{
	float yaw = glm::radians(euler.x);
	float pitch = glm::radians(euler.y);

	cameraFront.x = cos(pitch) * cos(yaw);
	cameraFront.y = sin(pitch);
	cameraFront.z = cos(pitch) * sin(yaw);
}

const glm::mat4& Camera::GetView()
{
	view = glm::lookAt(transform->GetPosition(), transform->GetPosition() + cameraFront, up );
	right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), -cameraFront));
	up = glm::normalize(glm::cross(right, cameraFront));
	return view;
}

const glm::vec3& Camera::GetDirection()
{
	return cameraFront;
}

const glm::vec3& Camera::GetUp()
{
	return up;
}

const glm::vec3& Camera::GetRight()
{
	return right;
}

void Camera::SetView(glm::mat4 view)
{
	this->view = view;
}
