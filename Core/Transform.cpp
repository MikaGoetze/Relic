#include "Transform.h"

void Transform::Update()
{
	if (parent != NULL)
		position = parent->GetPosition() + localPosition;
	else
		position = localPosition;

	if (parent != NULL)
		rotation = localRotation * parent->GetRotation();
	else
		rotation = localRotation;
}

void Transform::Start()
{
}

void Transform::Serialize()
{
	float lpx = localPosition.x;
	float lpy = localPosition.y;
	float lpz = localPosition.z;

	float px = position.x;
	float py = position.y;
	float pz = position.z;

	float lrx = localRotation.x;
	float lry = localRotation.y;
	float lrz = localRotation.z;
	float lrw = localRotation.w;

	float rx = rotation.x;
	float ry = rotation.y;
	float rz = rotation.z;
	float rw = rotation.w;

	//Lets serialize all of those
	Serializer::AddFloat("lpx", lpx);
	Serializer::AddFloat("lpy", lpy);
	Serializer::AddFloat("lpz", lpz);
	Serializer::AddFloat("px", px);
	Serializer::AddFloat("py", py);
	Serializer::AddFloat("pz", pz);
	Serializer::AddFloat("lrx", lrx);
	Serializer::AddFloat("lry", lry);
	Serializer::AddFloat("lrz", lrz);
	Serializer::AddFloat("lrw", lrw);
	Serializer::AddFloat("rx", rx);
	Serializer::AddFloat("ry", ry);
	Serializer::AddFloat("rz", rz);
	Serializer::AddFloat("rw", rw);

	Serializer::AddReference("parent", parent);
	Serializer::AddVector("children", children);
}

void Transform::Deserialize()
{
	localPosition.x = Serializer::GetFloat("lpx");
	localPosition.y = Serializer::GetFloat("lpy");
	localPosition.z = Serializer::GetFloat("lpz");
	position.x = Serializer::GetFloat("px");
	position.y = Serializer::GetFloat("py");
	position.z = Serializer::GetFloat("pz");
	localRotation.x = Serializer::GetFloat("lrx");
	localRotation.y = Serializer::GetFloat("lry");
	localRotation.z = Serializer::GetFloat("lrz");
	localRotation.w = Serializer::GetFloat("lrw");
	rotation.x = Serializer::GetFloat("rx");
	rotation.y = Serializer::GetFloat("ry");
	rotation.z = Serializer::GetFloat("rz");
	rotation.w = Serializer::GetFloat("rw");

	parent = Serializer::GetReference<Transform>("parent");
	children = Serializer::GetVector<Transform>("children");
}

glm::vec3 Transform::GetPosition()
{
	return position;
}

void Transform::SetPosition(glm::vec3 pos)
{
	glm::vec3 parentPos = parent->GetPosition();
	localPosition += (pos - parentPos) - (localPosition);
}

glm::vec3 Transform::GetLocalPosition()
{
	return localPosition;
}

void Transform::SetLocalPosition(glm::vec3 pos)
{
	localPosition = pos;
}

void Transform::SetRotation(glm::quat q)
{
	localRotation = q * glm::inverse(parent->GetRotation()); 
}

glm::quat Transform::GetRotation()
{
	return rotation;
}

void Transform::SetLocalRotation(glm::quat lq)
{
	localRotation = lq;
}

glm::quat Transform::GetLocalRotation()
{
	return localRotation;
}

Transform::Transform()
{
}
