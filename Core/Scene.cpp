#include "Scene.h"

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
}

void Scene::SetName(std::string name)
{
	this->name = name;
}
