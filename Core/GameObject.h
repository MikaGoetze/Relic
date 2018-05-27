#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <vector>
#include <Core/RelicBehaviour.h>

class GameObject
{
public:
	
	GameObject();
	GameObject(std::string name);
	~GameObject();

	template<typename T>
	T* GetComponent();

	template<typename T>
	T* AddComponent();

	std::string& Name;
private:
	std::string name;
	std::vector<RelicBehaviour*> behaviours;
};

template <typename T>
T* GameObject::GetComponent()
{
	for (RelicBehaviour* behaviour : behaviours)
	{
		if(typeid(behaviour) == typeid(T))
		{
			return behaviour;
		}
	}

	return NULL;
}

template <typename T>
T* GameObject::AddComponent()
{
	if(!std::is_base_of<RelicBehaviour, T>())
	{
		Util::Log("[Relic][GameObject] Warning : Class " + std::string(typeid(T).name()) + " does not inherit from RelicBehaviour. Discarding...");
		return NULL;
	}

	T* component = new T();
	behaviours.push_back(component);
	return component;
}

#endif
