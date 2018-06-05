#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <string>
#include <Core/Scene.h>
#include <fstream>
#include <vector>
#include <functional>
#include <map>

class Serializer
{
private:
	//Structs
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

		void Delete();

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

		XMLAttribute GetAttribute(std::string name);
	};

	struct Reference
	{
		void* pointer;
		unsigned int id;
		XMLObject* xml_obj;
	};

	//Vars
	std::ofstream ofs;
	unsigned int id;
	std::vector<Reference> References;

	XMLObject representation;
	static Serializer* instance;
	static std::map<std::string, std::function<RelicBehaviour*(void)>> behaviour_registry;

	//Functions
	//Writing
	void WriteXMLObject(XMLObject obj, int indent);
	void WriteXMLAttribute(XMLAttribute attrib, int indent);
	void WriteXMLArray(XMLArray array, int indent);
	void WriteID(XMLObject& obj);
	void WriteGO(GameObject* go, XMLArray& obj_array);
	void ResolveReferences(XMLObject& obj);

	static void DeleteXMLAttribute(XMLAttribute& attribute);
	//Reading
	XMLObject ReadXMLObject(std::ifstream& file);
	XMLAttribute ReadXMLAttribute(std::ifstream& file);
	XMLArray* ReadXMLArray(std::ifstream& file);
	std::string ReadLine(std::ifstream& file);
	bool AssertLine(std::ifstream& file, std::string line);
	std::string PeekLine(std::ifstream& file);
	std::string CleanWhiteSpace(std::string line);
	void ReconstructScene(Scene* scene, XMLObject& scene_obj);
	static RelicBehaviour* CreateRelicBehaviour(std::string type);
	RelicBehaviour* GetReferenceByID(int id);
	static void DeleteXMLObject(XMLObject* obj);
	static void DeleteXMLArray(XMLArray* arr);
	static std::string ValueTypeToString(ValueType type);

	void OpenOutputFile(std::string file);
	void CloseOutputFile();


	int GetIDByPointer(void* pointer);

	void AddAttribute(XMLObject& obj, XMLAttribute& attribute);
	XMLAttribute ConstructFloatAttribute(std::string name, float val);
	XMLAttribute ConstructIntAttribute(std::string name, int val);
	XMLAttribute ConstructBoolAttribute(std::string name, bool val);
	XMLAttribute ConstructStringAttribute(std::string name, std::string val);
	XMLAttribute ConstructObjectAttribute(std::string name, XMLObject *val);
	XMLAttribute ConstructArrayAttribute(std::string name, XMLArray* val);
public:

	Serializer();
	~Serializer();
	void WriteScene(Scene* scene);

	Scene* LoadScene(std::string filepath);

	void CreateXMLRepresentation();

	static void AddFloat(std::string name, float& val);
	static void AddString(std::string name, std::string& val);
	static void AddBool(std::string name, bool& val);
	static void AddInt(std::string name, int& val);

	static bool GetBool(std::string name);
	static float GetFloat(std::string name);
	static std::string GetString(std::string name);
	static int GetInt(std::string name);


	static void RegisterRelicBehaviour(std::string nam, std::function<RelicBehaviour*(void)> creator);

	template<typename T>
	static void AddVector(std::string name, std::vector<T*>& val);

	template<typename T>
	static std::vector<T*> GetVector(std::string name);

	template<typename T>
	static T* GetReference(std::string name);

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
			Util::Log("[Relic][Serializer] Error : Unsupported type encountered during serialization: " + std::string(typeid(T).name()));
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
std::vector<T*> Serializer::GetVector(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);
	
	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return std::vector<T*>();
	}

	if(attribute.GetValueType() != ValueType::XMLArray)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of array '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
		return std::vector<T*>();
	}

	XMLArray* arr = static_cast<XMLArray*>(attribute.GetValue());

	if(arr->type == ValueType::Reference)
	{
		//Then we need to resolve the references first
		std::vector<T*> ret_vctr;

		for(void* val : arr->elements)
		{
			RelicBehaviour* b = instance->GetReferenceByID(*static_cast<int*>(val));
			ret_vctr.push_back((T*) b);
		}
		return ret_vctr;
	}
	
	std::vector<T*> ret_vec;
	for(void* val : arr->elements)
	{
		ret_vec.push_back(static_cast<T*>(val));
	}
	//Otherwise we just return whatever was already loaded
	return static_cast<std::vector<T*>>(ret_vec);
}

template <typename T>
T* Serializer::GetReference(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);

	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return NULL;
	}

	if(attribute.GetValueType() != ValueType::Reference)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of reference '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
		return NULL;
	}

	return static_cast<T*>(GetReferenceByID(*static_cast<int*>(attribute.GetValue())));
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
