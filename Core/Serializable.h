#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable
{
public:
	virtual void Serialize() = 0;
	virtual void Deserialize() = 0;

	/*void AddFloat(std::string name, float& val)
	{
		Serializer::AddFloat(name, val);
	}

	void AddString(std::string name, std::string& val)
	{
		Serializer::AddString(name, val);
	}

	void AddBool(std::string name, bool& val)
	{
		Serializer::AddBool(name, val);
	}

	template<typename T>
	void AddVector(std::string name, std::vector<T*>& val)
	{
		Serializer::AddVector(name, val);
	}*/
};


#endif
