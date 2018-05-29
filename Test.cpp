#include "Test.h"
#include <iostream>
#include "Core/Serializer.h"

void Test::Serialize()
{
	Serializer::AddBool("b", b);
	Serializer::AddFloat("f", f);
	Serializer::AddVector("ia", ia);
}

void Test::Deserialize()
{
}

Test::Test()
{
}


Test::~Test()
{
}

void Test::Update()
{
}

void Test::Start()
{
}
