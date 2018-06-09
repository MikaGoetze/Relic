#include "Scene.h"
#include "Model/MeshRenderer.h"
#include "Texturing/RenderUtil.h"
#include "Relic.h"

Scene::Scene()
{
	skybox_shader = new Shader("Lighting/Shaders/skybox.vert", "Lighting/Shaders/skybox.frag");
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

void Scene::Render(Shader* shader, glm::mat4 proj, glm::mat4 view)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance_cubemap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter_cubemap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, brdf_lut);

	shader->SetInt("material.irradiance_map", 0);
	shader->SetInt("material.prefilter_map", 1);
	shader->SetInt("material.brdf_lut", 2);
	for(GameObject* go : gameObjects)
	{
		MeshRenderer* renderer = go->GetComponent<MeshRenderer>();
		if (renderer == NULL) continue;

		//Lets render it
		renderer->Render(shader);
	}

	//Now that we've done everything - lets render our skybox.

	glDepthFunc(GL_LEQUAL);
	skybox_shader->SetActive();

	//Bind the skybox textrue
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap);

	//Set relevant uniforms
	skybox_shader->SetMat4("projection", proj);
	skybox_shader->SetMat4("view", view);
	skybox_shader->SetInt("environment_map", 0);

	RenderUtil::RenderCube();

	Relic::GetStandardShader()->SetActive();
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

void Scene::SetSkybox(std::string HDR_Equirectangular_Env_Map_Path)
{
	//Get rid of cubemap seams.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	if(skyboxTexture != NULL)
	{
		Util::Log("[Relic][Scene] Warning : Discarding old skybox.");
		delete skyboxTexture;
	}
	skyboxTexture = new HDRTexture(HDR_Equirectangular_Env_Map_Path);

	//Lets convert it to a cubemap.
	skybox_cubemap = RenderUtil::RenderEquirectangularEnvironmentToCubemap(skyboxTexture->ID());

	//Calculate irradiance map based on skybox
	irradiance_cubemap = RenderUtil::ConvoluteCubemap(skybox_cubemap);

	//Calculate prefilter cubemap based on skybox
	prefilter_cubemap = RenderUtil::PreFilterEnvironment(skybox_cubemap);

	brdf_lut = RenderUtil::CalculateBRDF_LUT();
}
