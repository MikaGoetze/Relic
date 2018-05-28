#include "Serializer.h"
#include <Core/Util.h>

void Serializer::OpenOutputFile(std::string file)
{
	if(ofs.is_open())
	{
		Util::Log("[Relic][Serializer] Warning : Attempting to open output file while another is open. Close the open one first.");
		return;
	}

	ofs.open(file, std::ofstream::out | std::ofstream::trunc);
	id = 0;
}

void Serializer::CloseOutputFile()
{
	ofs.close();
}

void Serializer::WriteScene(Scene* scene)
{
	XMLObject xmlScene;
	AddAttribute(xmlScene, ConstructStringAttribute("name", scene->GetName()));
	WriteID(xmlScene);

	XMLArray gos;
	gos.type = ValueType::XMLObject;
	for (GameObject* go : scene->GetGameObjects())
	{
		WriteGO(go, gos);
	}

	AddAttribute(xmlScene, ConstructArrayAttribute("gos", &gos));

	OpenOutputFile(scene->GetName() + ".scene");
	WriteXMLObject(xmlScene);
	CloseOutputFile();
}

void Serializer::WriteGO(GameObject* go, XMLArray& obj_array)
{
	
}

void Serializer::WriteBehaviour(RelicBehaviour* behaviour)
{
}

void Serializer::AddAttribute(XMLObject& obj, XMLAttribute& attribute)
{

}

Serializer::XMLAttribute Serializer::ConstructFloatAttribute(std::string name, float val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val);
	return attribute;
}

Serializer::XMLAttribute Serializer::ConstructBoolAttribute(std::string name, bool val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val);
	return attribute;
}

Serializer::XMLAttribute Serializer::ConstructStringAttribute(std::string name, std::string val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val);
	return attribute;
}

Serializer::XMLAttribute Serializer::ConstructObjectAttribute(std::string name, XMLObject* val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val);
	return attribute;
}

Serializer::XMLAttribute Serializer::ConstructArrayAttribute(std::string name, XMLArray* val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val);
	return attribute;
}

void Serializer::WriteID(XMLObject& obj)
{
	AddAttribute(obj, ConstructFloatAttribute("id", id));
}

void* Serializer::XMLAttribute::GetValue()
{
	switch (type)
	{
	case ValueType::String: return static_cast<void*>(&value_s);
	case ValueType::Float: return static_cast<void*>(&value_f);
	case ValueType::Bool: return static_cast<void*>(&value_b);
	case ValueType::XMLObject: return static_cast<void*>(value_xo);
	case ValueType::XMLArray: return static_cast<void*>(value_xa);
	default: return NULL;
	}
}

Serializer::ValueType Serializer::XMLAttribute::GetValueType()
{
	return type;
}

void Serializer::XMLAttribute::SetValue(bool val)
{
	value_b = val;
	type = ValueType::Bool;
}

void Serializer::XMLAttribute::SetValue(float val)
{
	value_f = val;
	type = ValueType::Float;
}

void Serializer::XMLAttribute::SetValue(std::string val)
{
	value_s = val;
	type = ValueType::String;
}

void Serializer::XMLAttribute::SetValue(XMLObject* val)
{
	value_xo = val;
	type = ValueType::XMLObject;
}

void Serializer::XMLAttribute::SetValue(XMLArray* val)
{
	value_xa = val;
	type = ValueType::XMLArray;
}

const std::type_info& Serializer::XMLAttribute::GetType()
{
	switch (type)
	{
	case ValueType::String: return typeid(value_s);
	case ValueType::Float: return typeid(value_f);
	case ValueType::Bool: return typeid(value_b);
	case ValueType::XMLObject: return typeid(value_xo);
	case ValueType::XMLArray: return typeid(value_xa);
	default: return typeid(NULL);
	}
}

void Serializer::WriteXMLObject(XMLObject obj)
{
	ofs << "{" << std::endl;
	for(XMLAttribute attribute : obj.attributes)
	{
		WriteXMLAttribute(attribute);
		ofs << "," << std::endl;
	}
	ofs << "}" << std::endl;
}

void Serializer::WriteXMLAttribute(XMLAttribute attrib)
{
	ValueType type = attrib.GetValueType();

	XMLObject* obj;
	XMLArray* arr;

	switch (type)
	{
	case ValueType::String: 
		ofs << "\"" << attrib.name << "\": \"" << *static_cast<std::string*>(attrib.GetValue()) << "\"";
		break;
	case ValueType::Float: 
		ofs << "\"" << attrib.name << "\": " << *static_cast<float*>(attrib.GetValue());
		break;
	case ValueType::Bool:
		ofs << "\"" << attrib.name << "\": " << *static_cast<bool*>(attrib.GetValue());
		break;
	case ValueType::XMLObject:
		obj = static_cast<XMLObject*>(attrib.GetValue());
		WriteXMLObject(*obj);
		break;
	case ValueType::XMLArray: 
		ofs << "\"" << attrib.name << "\":" << std::endl;
		arr = static_cast<XMLArray*>(attrib.GetValue());
		WriteXMLArray(*arr);
		break;
	default: 
		ofs << "error: unknown type";
	}
}

void Serializer::WriteXMLArray(XMLArray array)
{
	ofs << "[" << std::endl;

	switch (array.type) { 
	case ValueType::String: 
		for (auto element : array.elements)
		{
			auto str = static_cast<std::string*>(element);
			ofs << "\"" << *str << "\"," << std::endl;;
		}
		break;
	case ValueType::Float:
		for (auto element : array.elements)
		{
			auto flt = static_cast<float*>(element);
			ofs << *flt << "," << std::endl;
		}
		break;
	case ValueType::Bool: 
		for(auto element : array.elements)
		{
			auto b = static_cast<bool*>(element);
			ofs << *b << "," << std::endl;
		}
		break;
	case ValueType::Reference: 
		for(auto element : array.elements)
		{
			auto ui = static_cast<unsigned*>(element);
			ofs << *ui << "," << std::endl;
		}
		break;
	case ValueType::XMLObject: 
		for(auto element : array.elements)
		{
			auto xo = static_cast<XMLObject*>(element);
			WriteXMLObject(*xo);
			ofs << "," << std::endl;
		}
	default: ;
	}

	ofs << "]" << std::endl;
}

