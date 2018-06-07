#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <Model/Model.h>
#include <GL/glew.h>
#include "Core/Util.h"

Model::Model()
{
	attrib = new tinyobj::attrib_t();
}

Model::Model(std::string filepath)
{
	attrib = new tinyobj::attrib_t();
	LoadModel(filepath);
}


Model::~Model()
{
}

void Model::LoadModel(std::string filepath)
{
	if(model_loaded)
	{
		Util::Log("[Relic][Model] Warning : Attempting to overwrite model. Ignoring...");
	}
	model_path = filepath;

	std::string err;
	std::string mat_directory = filepath.substr(0, filepath.find_last_of('/') + 1);
	bool res = tinyobj::LoadObj(attrib, &shapes, &materials, &err, filepath.c_str(), mat_directory.c_str(), true);

	if (res == false)
	{
		Util::Log("[Relic][Model] Error : Failed to load model - " + err);
		return;
	}

	//We will need to initialise the gpu vars
	vaos = std::vector<unsigned int>(shapes.size());
	vbos = std::vector<unsigned int>(shapes.size());
	on_gpu = std::vector<bool>(shapes.size());
	albedoTexs = std::vector<Texture>(materials.size());
	normalTexs = std::vector<Texture>(materials.size());
	metallicTexs = std::vector<Texture>(materials.size());
	roughnessTexs = std::vector<Texture>(materials.size());

	//Lets load the textures in
	for (int i = 0; i < materials.size(); i++)
	{
		tinyobj::material_t mat = GetMaterial(i);
		//Load the PBR textures
		LoadTexture(albedoTexs, i, mat.diffuse_texname, mat_directory);
		LoadTexture(normalTexs, i, mat.normal_texname, mat_directory);
		LoadTexture(metallicTexs, i, mat.metallic_texname, mat_directory);
		LoadTexture(roughnessTexs, i, mat.roughness_texname, mat_directory);
	}

	model_loaded = true;
	x_min = 10000000;
	x_max = 0;
	y_max = 0;
	y_min = 10000000;
	z_min = 10000000;
	z_max = 0;
}



bool Model::IsModelLoaded()
{
	return model_loaded;
}

std::string Model::GetName(unsigned int obj_index)
{
	if (!model_loaded) return "UNLOADED";

	return shapes.at(obj_index).name;
}

void Model::SendToGPU(unsigned int obj_index)
{
	//Lets convert the stored model to an array that we can pass

	if (!model_loaded) 
	{
		Util::Log("[Relic][Model] Warning : Model not loaded yet. Do not attempt to send to GPU.");
		return;
	}

	glGenVertexArrays(1, &vaos.at(obj_index));
	glBindVertexArray(vaos.at(obj_index));

	glGenBuffers(1, &vbos.at(obj_index));

	glBindBuffer(GL_ARRAY_BUFFER, vbos.at(obj_index));

	float * data = GenerateArray(obj_index);
	glBufferData(GL_ARRAY_BUFFER, shapes.at(obj_index).mesh.num_face_vertices.size() * 3 * 8 * sizeof(float), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	on_gpu.at(obj_index) = true;
}

unsigned Model::GetVAO(unsigned int obj_index)
{
	return vaos.at(obj_index);
}

unsigned Model::GetVBO(unsigned int obj_index)
{
	return vbos.at(obj_index);
}

glm::vec3 Model::GetDimensions()
{
	return glm::vec3((x_max - x_min), (y_max - y_min), (z_max - z_min));
}

int Model::GetNumShapes()
{
	return shapes.size();
}

int Model::GetNumVertices(unsigned int obj_index)
{
	return shapes.at(obj_index).mesh.indices.size();
}

int Model::GetTexID(unsigned obj_index)
{
	return albedoTexs.at(obj_index).ID();
}

int Model::GetNormalTexID(unsigned obj_index)
{
	return normalTexs.at(obj_index).ID();
}

int Model::GetMetallicTexID(unsigned obj_index)
{
	return metallicTexs.at(obj_index).ID();
}

int Model::GetRoughnessTexID(unsigned obj_index)
{
	return roughnessTexs.at(obj_index).ID();
}

std::string Model::GetModelPath()
{
	return model_path;
}

tinyobj::material_t Model::GetMaterial(unsigned obj_index)
{
	if (materials.empty() && model_loaded)
		return tinyobj::material_t();

	int ind = shapes.at(obj_index).mesh.material_ids.at(0);
	return materials.at(ind);
}


void Model::LoadTexture(std::vector<Texture>& tex_vec, int i, std::string filepath, std::string dir)
{
	if (filepath.empty())
	{
		Util::Log("[Relic][Model] Warning : Texture does not exist. Check that all models have appropriate PBR textures.");
		tex_vec.at(i) = Texture();
		return;
	}
	int ind = filepath.find('.');
	std::string filetype = filepath.substr(ind, filepath.size() - ind);
	tex_vec.at(i) = Texture(dir + filepath, filetype == ".png" ? GL_RGBA : GL_RGB);
}

float* Model::GenerateArray(unsigned int obj_index)
{
	tinyobj::mesh_t mesh = shapes.at(obj_index).mesh;

	float * data = new float[mesh.num_face_vertices.size() * 11 * 3];
	for(int i = 0; i < mesh.num_face_vertices.size() * 3; i++)
	{
		int index = 8 * i;
		//vertex pos
		data[index + 0] = attrib->vertices[3 * mesh.indices.at(i).vertex_index + 0];
		if (data[index] < x_min) x_min = data[index];
		if (data[index] > x_max) x_max = data[index];
		data[index + 1] = attrib->vertices[3 * mesh.indices.at(i).vertex_index + 1];
		if (data[index + 1] < y_min) y_min = data[index + 1];
		if (data[index + 1] > y_max) y_max = data[index + 1];
		data[index + 2] = attrib->vertices[3 * mesh.indices.at(i).vertex_index + 2];
		if (data[index + 2] < z_min) z_min = data[index + 2];
		if (data[index + 2] > z_max) z_max = data[index + 2];

		//normal pos
		data[index + 3] = attrib->normals[3 * mesh.indices.at(i).normal_index + 0];
		data[index + 4] = attrib->normals[3 * mesh.indices.at(i).normal_index + 1];
		data[index + 5] = attrib->normals[3 * mesh.indices.at(i).normal_index + 2];
		
		//tex coord
		//it's possible to not be textured, so lets take that into account
		try {
			data[index + 6] = attrib->texcoords.at(2 * mesh.indices.at(i).texcoord_index + 0);
			data[index + 7] = attrib->texcoords.at(2 * mesh.indices.at(i).texcoord_index + 1);
		}
		catch (std::out_of_range exception){}

		//Tangent calculation
		data[index + 8] =  

	}

	return data;

}
