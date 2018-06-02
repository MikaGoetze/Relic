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

int Serializer::GetIDByPointer(void* pointer)
{
	for(Reference ref : References)
	{
		if(ref.pointer == pointer)
		{
			return ref.id;
		}
	}
	return -1;
}

void Serializer::WriteScene(Scene* scene)
{
	id = 0;
	References = std::vector<Reference>();
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

	for (void* game_object : gos.elements)
	{
		XMLObject* obj = static_cast<XMLObject*>(game_object);

		ResolveReferences(*obj);
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

void Serializer::AddInt(std::string name, int& val)
{
	XMLAttribute attrib = instance->ConstructIntAttribute(name, val);
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

		//Lets also add it to the list
		const int obj_id = id;
		WriteID(*obj);

		Reference ref{};
		ref.id = obj_id;
		ref.pointer = static_cast<void*>(behaviour);

		References.push_back(ref);

		behaviours->elements.push_back(static_cast<void*>(obj));
	}

	obj_array.elements.push_back(static_cast<void*>(gameObj));
}

void Serializer::ResolveReferences(XMLObject& obj)
{
	References;
	for(int i = 0; i < obj.attributes.size(); i++)
	{
		XMLAttribute attrib = obj.attributes.at(i);

		if(attrib.GetValueType() == ValueType::Reference)
		{
			//We need to find a matching script with this pointer:
			void* p = attrib.GetReference();
			int id = GetIDByPointer(p);
			if(id == -1)
			{
				Util::Log("[Relic][Serializer] Error : Could not find script...");
				attrib.SetValue("ERR");
				continue;
			}

			attrib.SetValue(id, true);
		}
		else if(attrib.GetValueType() == ValueType::XMLArray)
		{
			XMLArray* array = static_cast<XMLArray*>(attrib.GetValue());
			if(array->type == ValueType::Reference)
			{
				XMLArray* newArray = new XMLArray();
				for(void* item : array->elements)
				{
					int id = GetIDByPointer(item);
					if(id == -1)
					{
						Util::Log("[Relic][Serializer] Error : Could not find script...");
					}
					int* id_p = new int();
					*id_p = id;
					newArray->elements.push_back(id_p);
				}
				newArray->type = ValueType::Reference;

				attrib.SetValue(newArray);
			}
			else if(array->type == ValueType::XMLObject)
			{
				for(void* item : array->elements)
					ResolveReferences(*static_cast<XMLObject*>(item));
			}
		}
		else if(attrib.GetValueType() == ValueType::XMLObject)
		{
			ResolveReferences(*static_cast<XMLObject*>(attrib.GetValue()));
		}

		obj.attributes.at(i) = attrib;
	}
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

Serializer::XMLAttribute Serializer::ConstructIntAttribute(std::string name, int val)
{
	XMLAttribute attribute;
	attribute.name = name;
	attribute.SetValue(val, false);
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
	id++;
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
	References = std::vector<Reference>();
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
	case ValueType::Reference:
	case ValueType::Int: return static_cast<void*>(&value_i);
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

void Serializer::XMLAttribute::SetValue(int val, bool is_reference)
{
	value_i = val;
	if (is_reference)
		type = ValueType::Reference;
	else
		type = ValueType::Int;
	
}

void Serializer::XMLAttribute::SetReference(void* val)
{
	ref_temp = val;
	type = ValueType::Reference;
}

void* Serializer::XMLAttribute::GetReference()
{
	return ref_temp;
}

const std::type_info& Serializer::XMLAttribute::GetType()
{
	switch (type)
	{
	case ValueType::String: return typeid(value_s);
	case ValueType::Float: return typeid(value_f);
	case ValueType::Bool: return typeid(value_b);
	case ValueType::Reference:
	case ValueType::Int: return typeid(value_i);
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
	case ValueType::Int:
	case ValueType::Reference:
		ofs << ind << "\"" << attrib.name << "\": " << *static_cast<int*>(attrib.GetValue());
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
	case ValueType::Int:
	case ValueType::Reference: 
		for(auto element : array.elements)
		{
			auto ui = static_cast<int*>(element);
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

