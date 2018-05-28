#include "Scene.h"

std::vector<GameObject*>& Scene::GetGameObjects()
{
	return gameObjects;
}

std::string & Scene::GetName()
{
	return name;
}
