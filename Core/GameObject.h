#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <vector>
#include <Core/Util.h>

class RelicBehaviour;

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

	template<typename T>
	void AddComponent(T* behaviour);

	std::string& Name;
private:
	std::string name;
	std::vector<RelicBehaviour*> behaviours;
	friend class Serializer;
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

template <typename T>
void GameObject::AddComponent(T* behaviour)
{
	if(!std::is_base_of<RelicBehaviour, T>())
	{
		Util::Log("[Relic][GameObject] Error : Attempting to add non RelicBehaviour '" + std::string(typeid(T).name()) + "' to GameObject.");
		return;
	}
	behaviours.push_back(behaviour);
}

#endif
