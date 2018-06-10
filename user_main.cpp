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
#include "Lighting/SpotLight.h"

MeshRenderer* renderer;
Shader* shader;
DirectionalLight* light;
PointLight* point_light;
SpotLight* spotLight;
Transform* model_transform;

bool follow_player = true;
bool point_locked = true;

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
	model_transform = tree->GetComponent<Transform>();
	MeshRenderer* renderer = tree->AddComponent<MeshRenderer>();
	renderer->scale = 5;
	renderer->SetGameObject(tree);
	Model* model = new Model("Models/gun2/gun.obj");
	for(int i = 0; i < model->GetNumShapes(); i++)
	{
		model->SendToGPU(i);
	}
	
	GameObject* go2 = new GameObject("go2");
	go2->GetComponent<Transform>()->SetPosition(glm::vec3(-2));
	MeshRenderer* r2 = go2->AddComponent<MeshRenderer>();
	r2->scale = 3;
	r2->SetGameObject(go2);
	Model* model2 = new Model("Models/gun2/gun.obj");
	for(int i = 0; i < model2->GetNumShapes(); i++)
	{
		model2->SendToGPU(i);
	}
	r2->SetModel(model2);

	glm::vec3 dims = model->GetDimensions();

	max = 0;
	if (dims.x > max) max = dims.x;
	if (dims.y > max) max = dims.y;
	if (dims.z > max) max = dims.z;

	player = new GameObject("Player");
	//player->GetComponent<Transform>()->SetPosition(glm::vec3(-2, -2, -2));
	Camera* cam = player->AddComponent<Camera>();
	cam->SetGameObject(player);
	FPSCamera* fpsCam = player->AddComponent<FPSCamera>();
	fpsCam->SetGameObject(player);

	PlayerMove* playerMove = player->AddComponent <PlayerMove>();
	playerMove->SetGameObject(player);

	scene->AddGameObject(player);
	scene->AddGameObject(go2);

	renderer->SetModel(model);

	scene->AddGameObject(tree);

	GameObject* child = new GameObject("child");
	child->GetComponent<Transform>()->SetParent(player->GetComponent<Transform>());

	Relic::GetStandardShader()->SetActive();
	light = new DirectionalLight(glm::vec3(-1, -1, -1), glm::vec3(0.5, 0.5, 0.5), 3, Relic::GetStandardShader());
	light->SetGameObject(player);
	player->AddComponent(light);
	spotLight = new SpotLight(glm::vec3(0), glm::vec3(0.4, 0.4, 0.4), 7, 12, 200, Relic::GetStandardShader());
	spotLight->SetGameObject(child);
	child->AddComponent(spotLight);
	scene->AddGameObject(child);

	point_light = new PointLight(glm::vec3(0.5, 0.5, 0.5), 20, Relic::GetStandardShader());
	point_light->SetActive(true);

	GameObject* pLight = new GameObject("P_LIGHT");
	pLight->AddComponent(point_light);
	point_light->SetGameObject(pLight);

	pLight->GetComponent<Transform>()->SetParent(player->GetComponent<Transform>());

	scene->AddGameObject(pLight);

	scene->SetSkybox("Textures/Alexs_Apt_2k.hdr");
	Serializer serializer;
	serializer.WriteScene(scene);

	Relic::LoadScene(scene);
}
	
void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
	glm::quat rot = glm::angleAxis((float) glm::radians(glfwGetTime() * 10), glm::vec3(0, 1.0, 0));
	model_transform->SetRotation(rot);

	if (Input::GetKeyDown(GLFW_KEY_SPACE))
	{
		Transform* spt = spotLight->GetGameObject()->GetComponent<Transform>();
		if(!follow_player)
		{
			spt->SetParent(player->GetComponent<Transform>());
			spt->SetLocalPosition(glm::vec3(0));
		}
		else
		{
			glm::vec3 pos = spt->GetPosition();
			spt->SetParent(NULL);
			spt->SetPosition(pos);
		}
		follow_player = !follow_player;
	}

	if (Input::GetKeyDown(GLFW_KEY_R))
	{
		Transform* ppt = point_light->GetGameObject()->GetComponent<Transform>();
		if(!point_locked)
		{
			ppt->SetParent(player->GetComponent<Transform>());
			ppt->SetLocalPosition(glm::vec3(0));
		}
		else
		{
			glm::vec3 pos = ppt->GetPosition();
			ppt->SetParent(NULL);
			ppt->SetPosition(pos);
		}
		point_locked = !point_locked;
	}

	if(follow_player)
		spotLight->SetDirection(player->GetComponent<FPSCamera>()->GetDirection());
	Relic::GetWindow()->SetWindowTitle("FPS : " + std::to_string(glm::round(Relic::GetSmoothedFPS())));

}