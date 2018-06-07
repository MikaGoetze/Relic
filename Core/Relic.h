#ifndef RELIC_H
#define RELIC_H

#define GLEW_STATIC
#include <gl/glew.h>
#include <string>
#include <GLFW/glfw3.h>
#include <Core/Window.h>
#include <vector>

extern void RelicEntry();
extern void RelicUpdate();

class RelicBehaviour;

class Relic
{
public:
	Relic();
	~Relic();

	static Window* GetWindow() { return instance->Window; }
	static void Exit();

private:
	static Relic* instance;
	Window* Window;
	bool gameRunning;

	void GameLoop();
	void Update();
	void Start();

	std::vector<RelicBehaviour*> * behaviours;
};


#endif