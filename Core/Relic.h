#ifndef RELIC_H
#define RELIC_H

#define GLEW_STATIC
#include <GL/glew.h>
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

	static Window* GetWindow() { return instance->window; }
	static void Exit();

	static void LoadScene(Scene* scene);

	static float GetDeltaTime();

	static Shader* GetStandardShader();

	static float GetSmoothedFPS();

	static Scene* GetCurrentScene();

private:
	static Relic* instance;
	Window* window;
	bool gameRunning;
	Scene* currentScene;
	Shader* standard_shader;

	float lastFrame, deltaTime;

	void GameLoop();
	void Update();
	void Start();

	static const int NUM_FPS_SAMPLES = 64;
	float fpsSamples[NUM_FPS_SAMPLES];
	int currentSample = 0;

	std::vector<RelicBehaviour*> * behaviours;
};


#endif
