// assignment3part2.cpp : Defines the entry point for the console application.
//

#include <Core/Relic.h>
#include "Test.h"
#include "Core/Input.h"

void RelicEntry()
{
	Relic::GetWindow()->Initialise(800, 600, "Relic Engine", true);
	Test* test = new Test();
	Relic::RegisterBehaviour(test);
}

void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
}