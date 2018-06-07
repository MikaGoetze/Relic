#include "Serializer.h"
#include <algorithm>
#include <Core/Util.h>
#include <Core/RelicBehaviour.h>

std::string Serializer::ReadLine(std::ifstream& file)
{
	std::string line;
	if(!std::getline(file, line))
	{
		Util::Log("[Relic][Serializer] Error : Unexpected EOF.");
		return "";
	}
	return line;
}

bool Serializer::AssertLine(std::ifstream& file, std::string line)
{
	std::string f_line = ReadLine(file);
	f_line = CleanWhiteSpace(f_line);
	if(f_line != line)
	{
		Util::Log("[Relic][Serializer] Error : Expected '" + line + "'. Received '" + f_line + "'.");
		return false;
	}
	return true;
}

std::string Serializer::PeekLine(std::ifstream& file)
{
	int pos = file.tellg();
	std::string line;
	std::getline(file, line);
	file.seekg(pos, std::ios_base::beg);

	return line;
}

std::string Serializer::CleanWhiteSpace(std::string line)
{
	std::string::iterator it = std::remove_if(line.begin(), line.end(), ::isspace);
	line.erase(it, line.end());
	return line;
}

void Serializer::ReconstructScene(Scene* scene, XMLObject& scene_obj)
{
	XMLAttribute scene_name_attr = scene_obj.GetAttribute("name");
	std::string name = *static_cast<std::string*>(scene_name_attr.GetValue());
	scene->SetName(name);

	References.clear();

	XMLArray* go_array = static_cast<XMLArray*>(scene_obj.GetAttribute("gos").GetValue());
	for(void* game_object : go_array->elements)
	{
		XMLObject* game_obj = static_cast<XMLObject*>(game_object);
		GameObject* go = new GameObject();

		go->name = *static_cast<std::string*>(game_obj->GetAttribute("name").GetValue());

		for (void* behaviour_void : static_cast<XMLArray*>(game_obj->GetAttribute("behaviours").GetValue())->elements)
		{
			XMLObject* obj_behaviour = static_cast<XMLObject*>(behaviour_void);

			//Lets figure out what kind of behaviour it is and then pass it off to be deserialized
			std::string behaviour_type = *static_cast<std::string*>(obj_behaviour->GetAttribute("_type_").GetValue());

			RelicBehaviour* behaviour = CreateRelicBehaviour(behaviour_type);
			behaviour->SetGameObject(go);

			Reference ref{};
			ref.id = *static_cast<int*>(obj_behaviour->GetAttribute("_id_").GetValue());
			ref.pointer = behaviour;
			ref.xml_obj = obj_behaviour;
			References.push_back(ref);

			go->AddComponent(behaviour);
		}

		scene->AddGameObject(go);
	}

	//Now that we've done the initial pass, lets deserialize our behaviours
	for(Reference ref : References)
	{
		RelicBehaviour* behaviour = static_cast<RelicBehaviour*>(ref.pointer);
		if(behaviour == NULL)
		{
			continue;
		}
		//Prepare for deserialization
		representation = *ref.xml_obj;

		behaviour->Deserialize();
	}
}

RelicBehaviour* Serializer::CreateRelicBehaviour(std::string type)
{
	try {
		return behaviour_registry.at(type)();
	}
	catch(std::exception e)
	{
		Util::Log("[Relic][Serializer] Error : Class '" + type + "' has not been registered and will not be loaded.");
		return NULL;
	}

}

RelicBehaviour* Serializer::GetReferenceByID(int id)
{
	for(Reference ref : References)
	{
		if(ref.id == id)
		{
			return static_cast<RelicBehaviour*>(ref.pointer);
		}
	}
	return NULL;
}

void Serializer::DeleteXMLObject(XMLObject* obj)
{
	for (XMLAttribute attribute : obj->attributes)
	{
		attribute.Delete();
	}
}

void Serializer::DeleteXMLArray(XMLArray* arr)
{
	for (void* val : arr->elements)
	{
		switch (arr->type)
		{
		case ValueType::XMLObject: 
			DeleteXMLObject(static_cast<XMLObject*>(val));
			break;
		default: ;
		}
	}

	delete arr;
}


std::string Serializer::ValueTypeToString(ValueType type)
{
	switch (type)
	{
	case ValueType::String: return "string";
	case ValueType::Float: return "float";
	case ValueType::Int: return "int";
	case ValueType::Bool: return "bool"; 
	case ValueType::Reference: return "Script Reference";
	case ValueType::XMLObject: return "Object";
	case ValueType::XMLArray: return "Array";
	default: ;
	}
}

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

Scene* Serializer::LoadScene(std::string filepath)
{
	Scene* scene = new Scene();

	std::ifstream file(filepath);

	//Load the scene from the XML Object
	XMLObject obj = ReadXMLObject(file);

	//Reconstruct the scene from the XML object
	ReconstructScene(scene, obj);

	//Lets clean up unneeded resources
	DeleteXMLObject(&obj);

	file.close();
	
	return scene;
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

bool Serializer::GetBool(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);
	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return NULL;
	}
	if(attribute.GetValueType() != ValueType::Bool)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of bool '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
	}

	return *static_cast<bool*>(attribute.GetValue());
}

float Serializer::GetFloat(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);
	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return float();
	}
	if(attribute.GetValueType() != ValueType::Float)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of float '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
	}

	return *static_cast<float*>(attribute.GetValue());
}

std::string Serializer::GetString(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);
	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return std::string();
	}
	if(attribute.GetValueType() != ValueType::String)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of string '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
	}

	return *static_cast<std::string*>(attribute.GetValue());
}

int Serializer::GetInt(std::string name)
{
	XMLAttribute attribute = instance->representation.GetAttribute(name);
	if(attribute.name == "ERR")
	{
		//Then the attribute doesnt exist.
		Util::Log("[Relic][Serializer] Warning : Attribute '" + name + "' does not exist.");
		return int();
	}
	if(attribute.GetValueType() != ValueType::Int)
	{
		Util::Log("[Relic][Serializer] Warning : Requested deserialization of int '" + name + "'. But it is of type: '" + ValueTypeToString(attribute.GetValueType()));
	};

	return *static_cast<int*>(attribute.GetValue());
}

void Serializer::RegisterRelicBehaviour(std::string name, std::function<RelicBehaviour*()> creator)
{
	behaviour_registry.insert(std::pair<std::string, std::function<RelicBehaviour*()>>(name, creator));
}


std::map<std::string, std::function<RelicBehaviour*(void)>> Serializer::behaviour_registry;

void Serializer::WriteGO(GameObject* go, XMLArray& obj_array)
{
	XMLObject* gameObj = new XMLObject();
	XMLArray* behaviours = new XMLArray();
	behaviours->type = ValueType::XMLObject;

	XMLAttribute array_attribute = XMLAttribute();
	array_attribute.name = "behaviours";
	array_attribute.SetValue(behaviours);

	XMLAttribute name_attribute = XMLAttribute();
	name_attribute.name = "name";
	name_attribute.SetValue(go->name);

	gameObj->attributes.push_back(name_attribute);
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

Serializer::XMLObject Serializer::ReadXMLObject(std::ifstream& file)
{
	std::string line;
	XMLObject obj;

	AssertLine(file, "{");
	while(CleanWhiteSpace(PeekLine(file)) != "}," && CleanWhiteSpace(PeekLine(file)) != "}")
	{
		XMLAttribute* attribute = new XMLAttribute();
		*attribute = ReadXMLAttribute(file);
		obj.attributes.push_back(*attribute);
	}
	ReadLine(file);
	
	return obj;
}


Serializer::XMLAttribute Serializer::ReadXMLAttribute(std::ifstream& file)
{
	XMLAttribute attribute;
	std::string line = ReadLine(file);
	line = CleanWhiteSpace(line);

	if(line[0] != '"')
	{
		Util::Log("[Relic][Core] Error : Expected quotatation mark. Got " + line + ".");
		return attribute;
	}
	line = line.substr(1, line.size() - 1);

	int pos = line.find('\"');
	std::string attr_name = line.substr(0, pos);
	attribute.name = attr_name;
	line = line.substr(pos + 2, line.size() - pos - 2);

	std::string val = line.substr(0, line.size() - 1);
	

	if(val[0] == '"')
	{
		//Then it's a string
		attribute.SetValue(val.substr(1, val.size() - 2));
	}
	else if(::isdigit(val[0]))
	{
		//Then it's a number - we'll have to check what type
		if(val.find('.') != -1)
		{
			//Then it's a float
			attribute.SetValue(static_cast<float>(atof(val.c_str())));
		}
		else
		{
			attribute.SetValue(static_cast<int>(atoi(val.c_str())), false);
		}
	}
	else if(val[0] == 'T' || val[0] == 'F')
	{
		//It's a bool
		attribute.SetValue(val[0] == 'T');
	}
	else if(val[0] == '#')
	{
		//Then it's a reference
		attribute.SetValue(static_cast<int>(atoi(val.substr(1, val.size() - 1).c_str())), false);
	}
	else if(val.size() == 0)
	{
		//Then it might be an array - lets peek the next line
		if(!AssertLine(file, "["))
		{
			return attribute;
		}

		//Lets read the array
		attribute.SetValue(ReadXMLArray(file));
	}

	if(line.size() > 0 && line[line.size() - 1] != ',')
	{
		Util::Log("[Relic][Core] Error : Expected comma. Got " + line + ".");
		return attribute;
	}

	return attribute;
}

Serializer::XMLArray *Serializer::ReadXMLArray(std::ifstream& file)
{
	XMLArray* arr = new XMLArray();

	std::string line = PeekLine(file);
	line = CleanWhiteSpace(line);

	if(line == "]")
	{
		//The array is empty so lets return.
		return arr;
	}

	ValueType type;
	void* item;

	if(line[0] == '"')
	{
		//Then it's a string
		type = ValueType::String;
		std::string* str = new std::string(line.substr(1, line.size() - 1));
		int pos = str->find('"');
		if(pos == -1)
		{
			Util::Log("[Relic][Serializer] Error : string literal was not closed correcetly. '" + line + "'.");
			return arr;
		}
		*str = str->substr(0, pos);
		arr->elements.push_back(static_cast<void*>(str));
		ReadLine(file);
	}
	else if(::isdigit(line[0]))
	{
		//Then it's a number - we'll have to check what type
		if(line.find('.') != -1)
		{
			//Then it's a float
			type = ValueType::Float;
			float* flt = new float();
			*flt = atof(line.substr(0, line.size() - 1).c_str());
			arr->elements.push_back(static_cast<void*>(flt));
		}
		else
		{
			type = ValueType::Int;
			int* i = new int();
			*i = atoi(line.substr(0, line.size() - 1).c_str());
			arr->elements.push_back(static_cast<void*>(i));
		}
		ReadLine(file);
	}
	else if(line[0] == 'T' || line[0] == 'F')
	{
		//It's a bool
		type = ValueType::Bool;
		bool *b = new bool();
		*b = line[0] == 'T';
		arr->elements.push_back(static_cast<void*>(b));
		ReadLine(file);
	}
	else if(line[0] == '#')
	{
		//Then it's a reference
		type = ValueType::Reference;
		int* id = new int();
		*id = atoi(line.substr(1, line.size() - 2).c_str());
		arr->elements.push_back(static_cast<void*>(id));
		ReadLine(file);
	}
	else if(line[0] == '{')
	{
		//Then it's an XMLObject.
		type = ValueType::XMLObject;
		XMLObject* obj = new XMLObject();
		*obj = ReadXMLObject(file);
		arr->elements.push_back(static_cast<void*>(obj));
	}

	arr->type = type;

	while(CleanWhiteSpace(PeekLine(file)) != "],")
	{
		line = CleanWhiteSpace(PeekLine(file));
		switch (type)
		{
		case ValueType::Bool:
		{
			bool* b = new bool();
			*b = line[0] == 'T';
			arr->elements.push_back(static_cast<void*>(b));
			ReadLine(file);
			break;
		}
		case ValueType::Float:
		{
			float* f = new float();
			*f = atof(line.substr(0, line.size() - 1).c_str());
			arr->elements.push_back(static_cast<void*>(f));
			ReadLine(file);
			break;
		}
		case ValueType::Int:
		{
			int* i = new int();
			*i = atoi(line.substr(0, line.size() - 1).c_str());
			arr->elements.push_back(static_cast<void*>(i));
			ReadLine(file);
			break;
		}
		case ValueType::Reference:
		{
			int* r = new int();
			*r = atoi(line.substr(1, line.size() - 2).c_str());
			arr->elements.push_back(static_cast<void*>(r));
			ReadLine(file);
			break;
		}
		case ValueType::String:
		{
			std::string *s = new std::string();
			*s = line.substr(1, line.size() - 2);
			arr->elements.push_back(static_cast<void*>(s));
			ReadLine(file);
			break;
		}
		case ValueType::XMLObject:
		{
			XMLObject * obj = new XMLObject();
			*obj = ReadXMLObject(file);
			arr->elements.push_back(static_cast<void*>(obj));
			break;
		}
		}
	}

	ReadLine(file);
	return arr;
	
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
	XMLAttribute attrib = ConstructFloatAttribute("_id_", id);
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

void Serializer::XMLAttribute::Delete()
{
	if(type == ValueType::XMLArray)
	{
		Serializer::DeleteXMLArray(value_xa);
	}

	else if(type == ValueType::XMLObject)
	{
		Serializer::DeleteXMLObject(value_xo);
	}
}

Serializer::XMLAttribute Serializer::XMLObject::GetAttribute(std::string name)
{
	for(XMLAttribute attr : attributes)
	{
		if(attr.name == name)
		{
			return attr;
		}
	}
	XMLAttribute attr;
	attr.name = "ERR";
	return attr;
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

	bool add_decimal = false;
	float flt;
	switch (type)
	{
	case ValueType::String: 
		ofs << ind << "\"" << attrib.name << "\": \"" << *static_cast<std::string*>(attrib.GetValue()) << "\"";
		break;
	case ValueType::Float: 
		flt = *static_cast<float*>(attrib.GetValue());
		if(fabsf(roundf(flt) - flt) < 0.00001f)
		{
			//Then its a round number so lets add a decmial place to it.
			add_decimal = true;
		}
		ofs << ind << "\"" << attrib.name << "\": " << *static_cast<float*>(attrib.GetValue()) << (add_decimal ? ".0" : "");
		break;
	case ValueType::Bool:
		ofs << ind << "\"" << attrib.name << "\": " << (*static_cast<bool*>(attrib.GetValue()) ? "T" : "F");
		break;
	case ValueType::Int:
		ofs << ind << "\"" << attrib.name << "\": " << *static_cast<int*>(attrib.GetValue());
		break;
	case ValueType::Reference:
		ofs << ind << "\"" << attrib.name << "\": #" << *static_cast<int*>(attrib.GetValue());
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

