#ifndef SCENE_H
#define SCENE_H
#include "GameObject.h"

class Scene
{
public:
	Scene();

	std::vector<GameObject*>& GetGameObjects();
	std::string& GetName();

private:
	std::string name;
	std::vector<GameObject*> gameObjects;
};

#endif