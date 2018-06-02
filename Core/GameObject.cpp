#include "GameObject.h"

void GameObject::Serialize()
{

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
