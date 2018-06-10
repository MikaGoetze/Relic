#include "Relic.h"
#include <iostream>
#include <Core/RelicBehaviour.h>
#include <Core/Input.h>
#include "Util.h"
#include <glm/gtc/matrix_transform.hpp>

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
	//Calculate delta time
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (Window->ShouldClose()) {
		gameRunning = false;
		return;
	}

	if(Input::Instance != NULL)
		Input::Update();

	currentScene->Update();

	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Hand control over to user
	RelicUpdate();

	//Do rendering stuff here.

	glm::mat4 view = Camera::main->GetView();
	glm::mat4 projection = glm::perspective(45.0f, (float) Window->WindowWidth() / Window->WindowHeight(), 0.05f, 100.0f);
	standard_shader->SetActive();
	standard_shader->SetMat4("view", view);
	standard_shader->SetMat4("projection", projection);
	standard_shader->SetVec3("viewPos", Camera::main->GetGameObject()->GetComponent<Transform>()->GetPosition());

	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	if(currentScene != NULL)
		currentScene->Render(standard_shader, projection, view, false);

	//We're done rendering, lets swap the buffers
	glfwSwapBuffers(Window->InternalWindow());
	//Poll events
	glfwPollEvents();

}

//Credit to khronos for this function
void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
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

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	if(err != GLEW_OK)
	{
		Util::Log("[Relic][Core] Error : Glew init failed. Extiting...");
		std::cout << "ERR : " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	//Load the standard shader
	standard_shader = new Shader("Lighting/Shaders/standard.vert", "Lighting/Shaders/standard.frag");

	//Initialise input
	Input::Instance = new Input(Window->InternalWindow());

	RelicStart();

	//We presume a scene has been loaded, lets start it.
	currentScene->Start();
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

void Relic::LoadScene(Scene* scene)
{
	instance->currentScene = scene;
}

float Relic::GetDeltaTime()
{
	return instance->deltaTime;
}

Shader* Relic::GetStandardShader()
{
	return instance->standard_shader;
}

float Relic::GetSmoothedFPS()
{
	instance->fpsSamples[instance->currentSample % NUM_FPS_SAMPLES] = 1.0f / instance->deltaTime;
	instance->currentSample++;
	float fps = 0;
	for (int i = 0; i < NUM_FPS_SAMPLES; i++)
	{
		fps += instance->fpsSamples[i];
	}
	fps /= NUM_FPS_SAMPLES;
	return fps;
}

Scene* Relic::GetCurrentScene()
{
	return instance->currentScene;
}
