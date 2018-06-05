#include "Test.h"
#include <iostream>

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
	b = Serializer::GetBool("b");
	f = Serializer::GetFloat("f");
	ia = Serializer::GetVector<float>("ia");
	others = Serializer::GetVector<Test>("others");
}

BehaviourRegistrar<Test> Test::registrar;

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
