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
		Int,
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

		void SetValue(int val, bool is_reference);

		void SetReference(void* val);

		void* GetReference();

		const std::type_info& GetType();

	private:
		bool value_b;
		float value_f;
		int value_i;
		std::string value_s;
		XMLObject *value_xo;
		XMLArray *value_xa;
		void* ref_temp;

		ValueType type;
	};

	struct XMLObject
	{
		std::vector<XMLAttribute> attributes;
	};

	struct Reference
	{
		void* pointer;
		unsigned int id;
	};

	void WriteXMLObject(XMLObject obj, int indent);
	void WriteXMLAttribute(XMLAttribute attrib, int indent);
	void WriteXMLArray(XMLArray array, int indent);

	void AddAttribute(XMLObject& obj, XMLAttribute& attribute);
	XMLAttribute ConstructFloatAttribute(std::string name, float val);
	XMLAttribute ConstructIntAttribute(std::string name, int val);
	XMLAttribute ConstructBoolAttribute(std::string name, bool val);
	XMLAttribute ConstructStringAttribute(std::string name, std::string val);
	XMLAttribute ConstructObjectAttribute(std::string name, XMLObject *val);
	XMLAttribute ConstructArrayAttribute(std::string name, XMLArray* val);
	void WriteID(XMLObject& obj);
	void WriteGO(GameObject* go, XMLArray& obj_array);
	void ResolveReferences(XMLObject& obj);
	void OpenOutputFile(std::string file);
	void CloseOutputFile();
	int GetIDByPointer(void* pointer);

	std::vector<Reference> References;

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
	static void AddInt(std::string name, int& val);

	template<typename T>
	static void AddVector(std::string name, std::vector<T*>& val);

	template<typename T>
	static void AddReference(std::string name, T* val);

};

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

template <typename T>
void Serializer::AddReference(std::string name, T* val)
{
	if (!std::is_base_of<RelicBehaviour, T>())
	{
		Util::Log("Unsupported type encountered during serialization: " + std::string(typeid(T).name()));
		return;
	}

	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetReference(val);

	instance->AddAttribute(instance->representation, attribute);
}

#endif
