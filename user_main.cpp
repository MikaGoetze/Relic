// user_main.cpp : Defines the main control point for Relic.
//

#include <Core/Relic.h>
#include "Test.h"
#include "Core/Input.h"
#include "Core/Serializer.h"
#include <iostream>
#include <algorithm>
#include "Model/Model.h"
#include "Model/MeshRenderer.h"
#include <glm/gtc/matrix_transform.inl>
#include "PlayerMove.h"

MeshRenderer* renderer;
Shader* shader;
void RelicEntry()
{
	Relic::GetWindow()->Initialise(800, 600, "Relic Engine", true);
}

void RelicStart()
{
	Scene* scene = new Scene();
	GameObject* tree = new GameObject("tree");
	MeshRenderer* renderer = tree->AddComponent<MeshRenderer>();
	renderer->SetGameObject(tree);
	Model* model = new Model("Models/Tree/PineTree03.obj");
	for(int i = 0; i < model->GetNumShapes(); i++)
	{
		model->SendToGPU(i);
	}
	
	GameObject* player = new GameObject("Player");
	Camera* cam = player->AddComponent<Camera>();
	cam->SetGameObject(player);
	FPSCamera* fpsCam = player->AddComponent<FPSCamera>();
	fpsCam->SetGameObject(player);
	PlayerMove* playerMove = player->AddComponent <PlayerMove>();
	playerMove->SetGameObject(player);

	scene->AddGameObject(player);

	renderer->SetModel(model);

	scene->AddGameObject(tree);

	Relic::LoadScene(scene);
}
	
void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();

}