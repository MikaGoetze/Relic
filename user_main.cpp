// user_main.cpp : Defines the main control point for Relic.
//

#include <Core/Relic.h>
#include "Test.h"
#include "Core/Input.h"
#include "Core/Serializer.h"
#include <iostream>

void RelicEntry()
{
	Relic::GetWindow()->Initialise(800, 600, "Relic Engine", true);

	//Let's test out our serializer
	Serializer* serializer = new Serializer();
	serializer->OpenOutputFile("test.txt");

	serializer->CloseOutputFile();
}

void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
}