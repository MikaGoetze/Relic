#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable
{
public:
	virtual void Serialize() = 0;
	virtual void Deserialize() = 0;
};


#endif
