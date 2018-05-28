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

	void WriteXMLObject(XMLObject obj);
	void WriteXMLAttribute(XMLAttribute attrib);
	void WriteXMLArray(XMLArray array);


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
public:
	void WriteScene(Scene* scene);

	//TODO: Implement these methods
	void CreateXMLRepresentation();
	void AddXMLRepresentation();

	void AddFloat(std::string name, float& val);
	void AddString(std::string name, std::string& val);
	void AddBool(std::string name, bool& val);

	template<typename T>
	void AddVector(std::string name, bool& val);

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

#endif
