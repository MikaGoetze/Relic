#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include <Core/GameObject.h>
#include <Lighting/Shader.h>
#include "Texturing/HDRTexture.h"

class Scene
{
public:
	Scene();

	std::vector<GameObject*>& GetGameObjects();
	std::string& GetName();

	void AddGameObject(GameObject* go);
	void SetName(std::string name);
	void Render(Shader* shader, glm::mat4 proj, glm::mat4 view, bool is_lighting_pass = false);
	void Update();
	void Start();

	void SetSkybox(std::string HDR_Equirectangular_Env_Map_Path);

private:
	bool has_started = false;
	bool has_skybox = false;
	unsigned int skybox_cubemap;
	unsigned int irradiance_cubemap;
	unsigned int prefilter_cubemap;
	unsigned int brdf_lut;
	std::string name;

	HDRTexture* skyboxTexture;
	std::vector<GameObject*> gameObjects;

	Shader* skybox_shader;
};

#endif