#include "Test.h"
#include <iostream>
#include "Core/Serializer.h"

void Test::Serialize()
{
	Serializer::AddBool("b", b);
	Serializer::AddFloat("f", f);
	Serializer::AddVector("ia", ia);
	if (others.size() > 0)
		Serializer::AddVector("others", others);
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
