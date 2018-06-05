#ifndef BEHAVIOUR_REGISTRAR_H
#define BEHAVIOUR_REGISTRAR_H
#include <string>
#include <Core/Serializer.h>

template<class T>
class BehaviourRegistrar
{
public:
	BehaviourRegistrar()
	{
		std::string class_name = typeid(T).name();
		int pos = class_name.find(' ');
		class_name = class_name.substr(pos + 1, class_name.size() - pos - 1);
		Serializer::RegisterRelicBehaviour(class_name, [](void)->RelicBehaviour* {return new T(); });
	}
};

#endif
