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
#include "Lighting/Light.h"
#include "Lighting/PointLight.h"
#include "Lighting/DirectionalLight.h"

MeshRenderer* renderer;
Shader* shader;
DirectionalLight* light;
PointLight* point_light;

float max;
GameObject* player;
void RelicEntry()
{
	Relic::GetWindow()->Initialise(800, 600, "Relic Engine", true);
}

void RelicStart()
{
	Scene* scene = new Scene();
	scene->SetName("Tree");
	GameObject* tree = new GameObject("tree");
	MeshRenderer* renderer = tree->AddComponent<MeshRenderer>();
	renderer->SetGameObject(tree);
	Model* model = new Model("Models/glock/Glock_17.obj");
	for(int i = 0; i < model->GetNumShapes(); i++)
	{
		model->SendToGPU(i);
	}
	
	glm::vec3 dims = model->GetDimensions();

	max = 0;
	if (dims.x > max) max = dims.x;
	if (dims.y > max) max = dims.y;
	if (dims.z > max) max = dims.z;

	player = new GameObject("Player");
	Camera* cam = player->AddComponent<Camera>();
	cam->SetGameObject(player);
	FPSCamera* fpsCam = player->AddComponent<FPSCamera>();
	fpsCam->SetGameObject(player);
	PlayerMove* playerMove = player->AddComponent <PlayerMove>();
	playerMove->SetGameObject(player);

	scene->AddGameObject(player);

	renderer->SetModel(model);

	scene->AddGameObject(tree);

	Serializer serializer;
	serializer.WriteScene(scene);

	Relic::GetStandardShader()->SetActive();
	light = new DirectionalLight(glm::vec3(-1, -1, -1), glm::vec3(1, 0.3, 0.3), 15, Relic::GetStandardShader());

	point_light = new PointLight(player->GetComponent<Transform>()->GetPosition(), glm::vec3(0.1, 0.1, 1), 1000, Relic::GetStandardShader());
	point_light->SetActive(true);

	light->SetActive(true);
	Relic::LoadScene(scene);
}
	
void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
	light->SetActive(true);
	point_light->SetPosition(glm::vec3(0, glm::sin(glfwGetTime()) * (max + 1), glm::cos(glfwGetTime()) * (max + 1)));


}