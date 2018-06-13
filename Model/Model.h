#ifndef MODEL_H
#define MODEL_H
#include <iostream>
#include <Model/tiny_obj_loader.h>
#include <vector>
#include <glm/detail/type_vec3.hpp>
#include "Texturing/Texture.h"


class Model
{
public:
	Model();
	Model(std::string filepath);
	~Model();

	void LoadModel(std::string filepath);
	bool IsModelLoaded();
	std::string GetName(unsigned int obj_index);

	void SendToGPU(unsigned int obj_index);

	unsigned int GetVAO(unsigned int obj_index);
	unsigned int GetVBO(unsigned int obj_index);

	glm::vec3 GetDimensions();

	int GetNumShapes();
	int GetNumVertices(unsigned int obj_index);
	int GetTexID(unsigned int obj_index);
	int GetNormalTexID(unsigned int obj_index);
	int GetMetallicTexID(unsigned int obj_index);
	int GetRoughnessTexID(unsigned int obj_index);

	std::string GetModelPath();

	tinyobj::material_t GetMaterial(unsigned int obj_index);

private:
	tinyobj::attrib_t* attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	bool model_loaded = false;
	std::vector<bool> on_gpu;
	std::vector<unsigned int> vaos;
	std::vector<unsigned int> vbos;
	std::vector<Texture> albedoTexs;
	std::vector<Texture> normalTexs;
	std::vector<Texture> metallicTexs;
	std::vector<Texture> roughnessTexs;
	std::vector<std::string> names;

	void LoadTexture(std::vector<Texture>& tex_vec, int i, std::string filepath, std::string dir);

	float x_min, x_max, y_min, y_max, z_min, z_max;

	float* GenerateArray(unsigned int obj_index);

	std::string model_path;
};

#endif //MODEL_H
