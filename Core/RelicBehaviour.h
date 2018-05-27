#ifndef RELIC_BEHAVIOUR_H
#define RELIC_BEHAVIOUR_H
#include <string>

class RelicBehaviour
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
