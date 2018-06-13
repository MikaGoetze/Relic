#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include <vector>
#include <typeinfo>
#include <Core/Util.h>

class RelicBehaviour;
class Transform;

class GameObject
{
public:
	GameObject(std::string name);
	~GameObject();

	template<typename T>
	T* GetComponent();

	template<typename T>
	T* AddComponent();

	template<typename T>
	void AddComponent(T* behaviour);

	Transform* transform;

	std::string& Name;
private:

	bool has_started = false;
	friend class Scene;
	GameObject();

	template<typename T>
	bool ContainsBehaviourType();

	std::string name;
	std::vector<RelicBehaviour*> behaviours;
	friend class Serializer;
};

template <typename T>
T* GameObject::GetComponent()
{
	for (RelicBehaviour* behaviour : behaviours)
	{
		if(typeid(*behaviour) == typeid(T))
		{
			return static_cast<T*>(behaviour);
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

template <typename T>
bool GameObject::ContainsBehaviourType()
{
	for(RelicBehaviour* behaviour : behaviours)
	{
		if(typeid(behaviour).name() == typeid(T).name())
		{
			return true;
		}
	}
	return false;
}

#endif
