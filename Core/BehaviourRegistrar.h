#ifndef BEHAVIOUR_REGISTRAR_H
#define BEHAVIOUR_REGISTRAR_H
#include <string>
#include <Core/Serializer.h>

template<class T>
class BehaviourRegistrar
{
public:
	BehaviourRegistrar(std::string name)
	{
		Serializer::RegisterRelicBehaviour(name, [](void)->RelicBehaviour* {return new T(); });
	}
};

#endif
