#ifndef RELIC_BEHAVIOUR_H
#define RELIC_BEHAVIOUR_H
#include <string>
#include <Core/Serializable.h>

class RelicBehaviour : Serializable
{
public:

	RelicBehaviour()
	{
	}

	virtual ~RelicBehaviour() = default;

	virtual void Update(){};
	virtual void Start(){};

};

#endif
