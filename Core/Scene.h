#ifndef SCENE_H
#define SCENE_H
#include "GameObject.h"

class Scene
{
public:
	Scene();

	std::vector<GameObject*>& GetGameObjects();
	std::string& GetName();

	void AddGameObject(GameObject* go);
	void SetName(std::string name);

private:
	std::string name;
	std::vector<GameObject*> gameObjects;
};

#endif