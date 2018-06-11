#include "PlayerMove.h"
#include "Core/Input.h"
#include <Core/Relic.h>

void PlayerMove::Serialize()
{
}

void PlayerMove::Deserialize()
{
}

void PlayerMove::Update()
{
	float speed = 8 * Relic::GetDeltaTime();
	if (Input::GetKey(GLFW_KEY_W)) 
		transform->SetPosition(transform->GetPosition() + cam->GetDirection() * speed);
	if (Input::GetKey(GLFW_KEY_S)) 
		transform->SetPosition(transform->GetPosition() - cam->GetDirection() * speed);
	if (Input::GetKey(GLFW_KEY_D)) 
		transform->SetPosition(transform->GetPosition() + cam->GetRight() * speed);
	if (Input::GetKey(GLFW_KEY_A)) 
		transform->SetPosition(transform->GetPosition() - cam->GetRight() * speed);
}

void PlayerMove::Start()
{
	cam = GetGameObject()->GetComponent<FPSCamera>();
	transform = GetGameObject()->GetComponent<Transform>();
}
