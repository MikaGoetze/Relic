#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <Core/GameObject.h>
#include <Lighting/Shader.h>

class Scene
{
public:
	Scene();

	std::vector<GameObject*>& GetGameObjects();
	std::string& GetName();

	void AddGameObject(GameObject* go);
	void SetName(std::string name);
	void Render(Shader* shader);
	void Update();
	void Start();

private:
	bool has_started = false;
	std::string name;
	std::vector<GameObject*> gameObjects;
};

#endif