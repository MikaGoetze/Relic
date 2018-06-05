#ifndef RELIC_BEHAVIOUR_H
#define RELIC_BEHAVIOUR_H
#include <Core/Serializable.h>
#include <Core/Serializer.h>
#include <Core/BehaviourRegistrar.h>

class RelicBehaviour : public Serializable
{
public:

	RelicBehaviour()
	{
	}

	virtual ~RelicBehaviour() = default;

	virtual void Update(){};
	virtual void Start(){};

	static std::string GetClassName(RelicBehaviour* behaviour);

};

inline std::string RelicBehaviour::GetClassName(RelicBehaviour* behaviour)
{
	return typeid(behaviour).name();
}

#endif
