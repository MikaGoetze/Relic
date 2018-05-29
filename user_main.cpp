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

	Scene* scene = new Scene();
	scene->SetName("test_scene");

	GameObject* go = new GameObject("test_go");
	Test* test = go->AddComponent<Test>();

	test->b = true;
	test->f = 0.05f;
	float* a = new float();
	*a = 2;
	float* b = new float();
	*b = 3;
	float* c = new float();
	*c = 4;

	test->ia.push_back(a);
	test->ia.push_back(b);
	test->ia.push_back(c);

	scene->AddGameObject(go);

	serializer->WriteScene(scene);
}

void RelicUpdate()
{
	if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) Relic::Exit();
}