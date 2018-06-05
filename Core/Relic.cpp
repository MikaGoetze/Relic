#include "Relic.h"
#include <iostream>
#include <Core/RelicBehaviour.h>
#include <Core/Input.h>
#include "Util.h"

Relic * Relic::instance;


Relic::Relic()
{
	if(instance != NULL)
	{
		Util::Log("[Relic][Core] Warning : Attemped to initialise a second engine instance. Discarding...");
		this->~Relic();
		return;
	}

	//Do initialisation here

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	glewInit();

	Window = new ::Window();
	behaviours = new std::vector<RelicBehaviour*>();

	//Lets set the game running
	gameRunning = true;

	//Clear the editor log
	Util::CleanLogFile();

	instance = this;
	Util::Log("[Relic][Core] Initialised Engine. Starting game loop...");

	//Let's start the game loop now
	GameLoop();
}


Relic::~Relic()
{
}

void Relic::GameLoop()
{
	Start();
	while(gameRunning)
	{
		Update();
	}
}

void Relic::Update()
{
	if (Window->ShouldClose()) {
		gameRunning = false;
		return;
	}

	if(Input::Instance != NULL)
		Input::Update();

	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Hand control over to user
	RelicUpdate();

	//Do rendering stuff here.


	//We're done rendering, lets swap the buffers
	glfwSwapBuffers(Window->InternalWindow());
	//Poll events
	glfwPollEvents();

}

void Relic::Start()
{
	//Pass control over to the user for initialisation
	RelicEntry();

	if(!Window->initialised)
	{
		Util::Log("[Relic][Core] Error : Finished initialisation, but no game window is set up. Extiting...");
		Util::ErrorExit();
	}

	//Initialise input
	Input::Instance = new Input(Window->InternalWindow());
}

void Relic::Exit()
{
	glfwSetWindowShouldClose(instance->Window->InternalWindow(), true);
	for (auto& behaviour : *instance->behaviours)
	{
		Util::Log("[Relic][Core] Cleaning up " + RelicBehaviour::GetClassName(behaviour) + ".");
		delete behaviour;
	}
	instance->gameRunning = false;
}

void Relic::RegisterBehaviour(RelicBehaviour* behaviour)
{
	instance->Register(behaviour);
}

void Relic::Register(RelicBehaviour* behaviour)
{
	Util::Log("[Relic][Core] Registered " + RelicBehaviour::GetClassName(behaviour));
	behaviours->push_back(behaviour);
}




