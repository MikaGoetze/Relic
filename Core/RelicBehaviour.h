#ifndef RELIC_BEHAVIOUR_H
#define RELIC_BEHAVIOUR_H
#include <Core/Serializable.h>
#include <Core/Serializer.h>
#include <Core/BehaviourRegistrar.h>
#include <Core/GameObject.h>

class RelicBehaviour : public Serializable
{
public:

	virtual ~RelicBehaviour() = default;

	virtual void Update(){};
	virtual void Start(){};

	static std::string GetClassName(RelicBehaviour* behaviour);

	GameObject* GetGameObject();
	void SetGameObject(GameObject* go);
private:
	GameObject * game_object;
};

inline std::string RelicBehaviour::GetClassName(RelicBehaviour* behaviour)
{
	return typeid(behaviour).name();
}

inline GameObject* RelicBehaviour::GetGameObject()
{
	return game_object;
}

inline void RelicBehaviour::SetGameObject(GameObject* go)
{
	game_object = go;
}
#endif
