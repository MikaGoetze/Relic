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

	tinyobj::material_t GetMaterial(unsigned int obj_index);

private:
	tinyobj::attrib_t* attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	bool model_loaded;
	std::vector<bool> on_gpu;
	std::vector<unsigned int> vaos;
	std::vector<unsigned int> vbos;
	std::vector<Texture> texs;

	float x_min, x_max, y_min, y_max, z_min, z_max;

	float* GenerateArray(unsigned int obj_index);
};

#endif //MODEL_H