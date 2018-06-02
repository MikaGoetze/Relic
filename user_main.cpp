// user_main.cpp : Defines the main control point for Relic.
//

#include <Core/Relic.h>
#include "Test.h"
#include "Core/Input.h"
#include "Core/Serializer.h"
#include <iostream>
#include <algorithm>

void RelicEntry()
{
	Relic::GetWindow()->Initialise(800, 600, "Relic Engine", true);

	//Let's test out our serializer
	Serializer* serializer = new Serializer();

	Scene* scene = serializer->LoadScene("test_scene.scene");

	
}

void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
}