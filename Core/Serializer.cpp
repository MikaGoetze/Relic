﻿#include "Serializer.h"
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
	XMLAttribute attrib = ConstructStringAttribute("name", scene->GetName());
	AddAttribute(xmlScene, attrib);
	WriteID(xmlScene);

	XMLArray gos;
	gos.type = ValueType::XMLObject;
	for (GameObject* go : scene->GetGameObjects())
	{
		WriteGO(go, gos);
	}

	attrib = ConstructArrayAttribute("gos", &gos);
	AddAttribute(xmlScene, attrib);

	OpenOutputFile(scene->GetName() + ".scene");
	WriteXMLObject(xmlScene, 0);
	CloseOutputFile();
}

void Serializer::CreateXMLRepresentation()
{
	representation = XMLObject();
}

void Serializer::AddFloat(std::string name, float& val)
{
	XMLAttribute attrib = instance->ConstructFloatAttribute(name, val);
	instance->AddAttribute(instance->representation, attrib);
}

void Serializer::AddString(std::string name, std::string& val)
{
	XMLAttribute attrib = instance->ConstructStringAttribute(name, val);
	instance->AddAttribute(instance->representation, attrib);
}

void Serializer::AddBool(std::string name, bool& val)
{
	XMLAttribute attrib = instance->ConstructBoolAttribute(name, val);
	instance->AddAttribute(instance->representation, attrib);
}

void Serializer::WriteGO(GameObject* go, XMLArray& obj_array)
{
	XMLObject* gameObj = new XMLObject();
	XMLArray* behaviours = new XMLArray();
	behaviours->type = ValueType::XMLObject;

	XMLAttribute array_attribute = XMLAttribute();
	array_attribute.name = "behaviours";
	array_attribute.SetValue(behaviours);

	gameObj->attributes.push_back(array_attribute);

	for(RelicBehaviour* behaviour : go->behaviours)
	{
		Util::Log("[Relic][Core] Serializing " + std::string(typeid(*behaviour).name()));
		CreateXMLRepresentation();

		behaviour->Serialize();

		//Add it's type for later
		std::string class_name = std::string((typeid(*behaviour).name()));
		class_name = class_name.substr(6, class_name.size() - 6);
		XMLAttribute attrib = ConstructStringAttribute("_type_", class_name);
		AddAttribute(representation, attrib);

		XMLObject* obj = new XMLObject(representation);

		behaviours->elements.push_back(static_cast<void*>(obj));
	}

	obj_array.elements.push_back(static_cast<void*>(gameObj));
}

void Serializer::AddAttribute(XMLObject& obj, XMLAttribute& attribute)
{
	obj.attributes.push_back(attribute);
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
	XMLAttribute attrib = ConstructFloatAttribute("id", id);
	AddAttribute(obj, attrib);
}

Serializer::Serializer()
{
	if(instance != NULL)
	{
		Util::Log("[Relic][Serializer] Warning : Serializer instance is already initialised. Discarding new copy...");
		return;
	}
	instance = this;
}

Serializer::~Serializer()
{
}

Serializer* Serializer::instance;

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

void Serializer::WriteXMLObject(XMLObject obj, int indent)
{
	std::string ind = std::string(indent, ' ');
	ofs << ind << "{" << std::endl;
	for(XMLAttribute attribute : obj.attributes)
	{
		WriteXMLAttribute(attribute, indent + 1);
		ofs << "," << std::endl;
	}
	ofs << ind << "}";
}

void Serializer::WriteXMLAttribute(XMLAttribute attrib, int indent)
{
	ValueType type = attrib.GetValueType();

	std::string ind = std::string(indent, ' ');

	XMLObject* obj;
	XMLArray* arr;

	switch (type)
	{
	case ValueType::String: 
		ofs << ind << "\"" << attrib.name << "\": \"" << *static_cast<std::string*>(attrib.GetValue()) << "\"";
		break;
	case ValueType::Float: 
		ofs << ind << "\"" << attrib.name << "\": " << *static_cast<float*>(attrib.GetValue());
		break;
	case ValueType::Bool:
		ofs << ind << "\"" << attrib.name << "\": " << *static_cast<bool*>(attrib.GetValue());
		break;
	case ValueType::XMLObject:
		obj = static_cast<XMLObject*>(attrib.GetValue());
		WriteXMLObject(*obj, indent + 1);
		break;
	case ValueType::XMLArray: 
		ofs << ind << "\"" << attrib.name << "\":" << std::endl;
		arr = static_cast<XMLArray*>(attrib.GetValue());
		WriteXMLArray(*arr, indent + 1);
		break;
	default: 
		ofs << ind << "error: unknown type";
	}
}

void Serializer::WriteXMLArray(XMLArray array, int indent)
{
	std::string ind = std::string(indent, ' ');
	ofs << std::string(indent - 1, ' ') << "[" << std::endl;

	switch (array.type) { 
	case ValueType::String: 
		for (auto element : array.elements)
		{
			auto str = static_cast<std::string*>(element);
			ofs << ind << "\"" << *str << "\"," << std::endl;;
		}
		break;
	case ValueType::Float:
		for (auto element : array.elements)
		{
			auto flt = static_cast<float*>(element);
			ofs << ind << *flt << "," << std::endl;
		}
		break;
	case ValueType::Bool: 
		for(auto element : array.elements)
		{
			auto b = static_cast<bool*>(element);
			ofs << ind << *b << "," << std::endl;
		}
		break;
	case ValueType::Reference: 
		for(auto element : array.elements)
		{
			auto ui = static_cast<unsigned*>(element);
			ofs << ind << *ui << "," << std::endl;
		}
		break;
	case ValueType::XMLObject: 
		for(auto element : array.elements)
		{
			auto xo = static_cast<XMLObject*>(element);
			WriteXMLObject(*xo, indent + 1);
			ofs << "," << std::endl;
		}
	default: ;
	}

	ofs << std::string(indent - 1, ' ') << "]";
}

