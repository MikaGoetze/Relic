#ifndef PLAYER_MOVE_H
#define PLAYER_MOVE_H
#include <Core/RelicBehaviour.h>
#include <Camera/FPSCamera.h>
#include <Core/Transform.h>

class PlayerMove : public RelicBehaviour
{
public:
	void Serialize() override;
	void Deserialize() override;
	void Update() override;
	void Start() override;
	PlayerMove() = default;
private:
	FPSCamera * cam = NULL;
	Transform* transform = NULL;
};

#endif
