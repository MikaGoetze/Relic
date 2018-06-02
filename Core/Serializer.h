#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <string>
#include "Scene.h"
#include <fstream>

class Serializer
{
private:


	std::ofstream ofs;
	unsigned int id;

	struct XMLObject;

	enum class ValueType
	{
		String,
		Float,
		Bool,
		Reference,
		XMLObject,
		XMLArray
	};

	struct XMLArray
	{
		ValueType type;
		std::vector<void*> elements;
	};

	struct XMLAttribute
	{
	public:
		std::string name;

		XMLAttribute()
		{
			value_xa = NULL;
			value_xo = NULL;
		}

		void* GetValue();

		ValueType GetValueType();

		void SetValue(bool val);

		void SetValue(float val);

		void SetValue(std::string val);

		void SetValue(XMLObject* val);

		void SetValue(XMLArray* val);

		const std::type_info& GetType();

	private:
		bool value_b;
		float value_f;
		std::string value_s;
		XMLObject *value_xo;
		XMLArray *value_xa;

		ValueType type;
	};

	struct XMLObject
	{
		std::vector<XMLAttribute> attributes;
	};

	void WriteXMLObject(XMLObject obj, int indent);
	void WriteXMLAttribute(XMLAttribute attrib, int indent);
	void WriteXMLArray(XMLArray array, int indent);


	//template<typename T>
	//void WriteAttribute(std::string name, T& value);

	template<typename T>
	void WriteArray(std::string name, std::string type, std::vector<T*>& values);

	void AddAttribute(XMLObject& obj, XMLAttribute& attribute);
	XMLAttribute ConstructFloatAttribute(std::string name, float val);
	XMLAttribute ConstructBoolAttribute(std::string name, bool val);
	XMLAttribute ConstructStringAttribute(std::string name, std::string val);
	XMLAttribute ConstructObjectAttribute(std::string name, XMLObject *val);
	XMLAttribute ConstructArrayAttribute(std::string name, XMLArray* val);
	void WriteID(XMLObject& obj);
	void WriteGO(GameObject* go, XMLArray& obj_array);
	void WriteBehaviour(RelicBehaviour* behaviour);
	void OpenOutputFile(std::string file);
	void CloseOutputFile();

	XMLObject representation;
	static Serializer* instance;
public:

	Serializer();
	~Serializer();
	void WriteScene(Scene* scene);

	void CreateXMLRepresentation();

	static void AddFloat(std::string name, float& val);
	static void AddString(std::string name, std::string& val);
	static void AddBool(std::string name, bool& val);

	template<typename T>
	static void AddVector(std::string name, std::vector<T*>& val);

};

/*template <typename T>
void Serializer::WriteAttribute(std::string name, T& value)
{
	ofs << "\"" << name << "\": \"" << std::to_string(value) << "\",\n";
}*/

template <typename T>
void Serializer::WriteArray(std::string name, std::string type, std::vector<T*>& values)
{
	ofs << "\"_array_\": {\n\"name\": \"" << name << "\",\n\"type\":" << type << ",\n\"values\": [\n";
	for(T* element : values)
	{
		
	}
	ofs << "]\n},\n";
}

template <typename T>
void Serializer::AddVector(std::string name, std::vector<T*>& val)
{
	XMLArray *array = new XMLArray();
	std::string type = std::string(typeid(T).name());
	
	if (type == "float")
		array->type = ValueType::Float;
	else if(type == "std::string")
		array->type = ValueType::String;
	else if(type == "bool")
		array->type = ValueType::Bool;
	else{
		if(std::is_base_of<RelicBehaviour, T>())
		{
			array->type = ValueType::Reference;
		}
		else
		{
			Util::Log("Unsupported type encountered during serialization: " + std::string(typeid(T).name()));
			return;
		}
	}

	for(int i = 0; i < val.size(); i++)
	{
		array->elements.push_back(static_cast<void*>(val.at(i)));
	}

	XMLAttribute attrib = instance->ConstructArrayAttribute(name, array);
	instance->AddAttribute(instance->representation, attrib);
}

#endif
