#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include <Model/Model.h>
#include "Core/RelicBehaviour.h"
#include "Lighting/Shader.h"

class MeshRenderer : public RelicBehaviour
{
public:
	void Serialize() override;
	void Deserialize() override;
private:
	Model * model = NULL;
public:
	void Render(Shader* shader);

	Model* SetModel(Model* model);
	Model* GetModel();
	
};

#endif
