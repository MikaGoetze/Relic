#include "GameObject.h"
#include <Core/Transform.h>

//Useful for deserialization, does not automatically add a transform
GameObject::GameObject() : Name(name)
{
	behaviours = std::vector<RelicBehaviour*>();
	name = "GameObject";
}

//User function - creates gameobject with a given name. Also automatically adds a transform.
GameObject::GameObject(std::string name) : Name(name)
{
	behaviours = std::vector<RelicBehaviour*>();
	this->name = name;
	AddComponent<Transform>();
}

GameObject::~GameObject()
{
	for (RelicBehaviour* behaviour : behaviours)
	{
		delete behaviour;
	}
}

