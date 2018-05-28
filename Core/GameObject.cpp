#include "GameObject.h"
#include <fstream>

void GameObject::Serialize()
{
	std::string scene = "test.txt";
	std::ofstream ofs;
	ofs.open(scene.c_str(), std::ofstream::out | std::ofstream::app);



	ofs.close();
}

void GameObject::Deserialize()
{
}

GameObject::GameObject() : Name(name)
{
	behaviours = std::vector<RelicBehaviour*>();
	name = "GameObject";
}

GameObject::GameObject(std::string name) : Name(name)
{
	behaviours = std::vector<RelicBehaviour*>();
	this->name = name;
}

GameObject::~GameObject()
{
	for (RelicBehaviour* behaviour : behaviours)
	{
		delete behaviour;
	}
}
