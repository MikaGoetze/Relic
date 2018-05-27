#include "GameObject.h"

GameObject::GameObject()
{
	behaviours = std::vector<RelicBehaviour*>();
	name = "GameObject";
	Name = name;
}

GameObject::GameObject(std::string name)
{
	behaviours = std::vector<RelicBehaviour*>();
	this->name = name;
	Name = this->name;
}

GameObject::~GameObject()
{
	for (RelicBehaviour* behaviour : behaviours)
	{
		delete behaviour;
	}
}
