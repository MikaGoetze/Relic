#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <Core/RelicBehaviour.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform : public RelicBehaviour
{
private:
	glm::vec3 position;
	glm::vec3 localPosition;

	glm::quat rotation;
	glm::quat localRotation;

	std::vector<Transform*> children;
	Transform* parent = NULL;

	void Update() override;
	void Start() override;
	void Serialize() override;
	void Deserialize() override;

public:
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 pos);

	glm::vec3 GetLocalPosition();
	void SetLocalPosition(glm::vec3 pos);

	void SetRotation(glm::quat q);
	glm::quat GetRotation();

	void SetLocalRotation(glm::quat lq);
	glm::quat GetLocalRotation();

	Transform();
	
};

#endif
