#ifndef RELIC_H
#define RELIC_H

#define GLEW_STATIC
#include <gl/glew.h>
#include <string>
#include <GLFW/glfw3.h>
#include <Core/Window.h>
#include <vector>
#include <Camera/FPSCamera.h>
#include <Core/Scene.h>

extern void RelicEntry();
extern void RelicUpdate();
extern void RelicStart();

class RelicBehaviour;

class Relic
{
public:
	Relic();
	~Relic();

	static Window* GetWindow() { return instance->Window; }
	static void Exit();

	static void LoadScene(Scene* scene);

	static float GetDeltaTime();

private:
	static Relic* instance;
	Window* Window;
	bool gameRunning;
	Scene* currentScene;
	Shader* standard_shader;

	float lastFrame, deltaTime;

	void GameLoop();
	void Update();
	void Start();

	std::vector<RelicBehaviour*> * behaviours;
};


#endif