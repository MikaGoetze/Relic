#include "Scene.h"
#include "Model/MeshRenderer.h"

Scene::Scene()
{
}

std::vector<GameObject*>& Scene::GetGameObjects()
{
	return gameObjects;
}

std::string & Scene::GetName()
{
	return name;
}

void Scene::AddGameObject(GameObject* go)
{
	gameObjects.push_back(go);
	if(!go->has_started && has_started)
	{
		for(RelicBehaviour* behaviour : go->behaviours)
		{
			behaviour->Start();
		}
		go->has_started = true;
	}
}

void Scene::SetName(std::string name)
{
	this->name = name;
}

void Scene::Render(Shader* shader)
{
	for(GameObject* go : gameObjects)
	{
		MeshRenderer* renderer = go->GetComponent<MeshRenderer>();
		if (renderer == NULL) continue;

		//Lets render it
		renderer->Render(shader);
	}
}

void Scene::Update()
{
	for(GameObject* go : gameObjects)
	{
		for(RelicBehaviour* behaviour : go->behaviours)
		{
			behaviour->Update();
		}
	}
}

void Scene::Start()
{
	has_started = true;
	for(GameObject* go : gameObjects)
	{
		for(RelicBehaviour* behaviour : go->behaviours)
		{
			behaviour->Start();
		}
		go->has_started = true;
	}
}
