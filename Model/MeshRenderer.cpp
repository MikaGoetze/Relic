#include "MeshRenderer.h"
#include <Core/Relic.h>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Core/Transform.h>


void MeshRenderer::Serialize()
{
	std::string model_path = model->GetModelPath();
	Serializer::AddString("model_path", model_path);
}

void MeshRenderer::Deserialize()
{
	std::string model_path = Serializer::GetString("model_path");

	model = new Model(model_path);
}

void MeshRenderer::Render(Shader* shader)
{

	for (int i = 0; i < model->GetNumShapes(); i++)
	{
		glBindVertexArray(model->GetVAO(i));

		tinyobj::material_t mat = model->GetMaterial(i);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, model->GetTexID(i));

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, model->GetNormalTexID(i));

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, model->GetRoughnessTexID(i));

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, model->GetMetallicTexID(i));

		//Update our mesh's position based on scene hierarchy.
		Transform* transform = GetGameObject()->GetComponent<Transform>();

		glm::mat4 mod = glm::mat4();
		mod = glm::translate(mod, transform->GetPosition());
		mod = glm::toMat4(transform->GetRotation()) * mod;

		shader->SetMat4("model", mod);
		shader->SetInt("material.albedo", 3);
		shader->SetInt("material.normal", 4);
		shader->SetInt("material.roughness", 5);
		shader->SetInt("material.metallic", 6);
		shader->SetFloat("material.shininess", mat.shininess);
		shader->SetVec3("material.ambient", mat.ambient[0], mat.ambient[1], mat.ambient[2]);

		glDrawArrays(GL_TRIANGLES, 0, model->GetNumVertices(i));
	}
}

Model* MeshRenderer::SetModel(Model* model)
{
	Model* old = this->model;
	this->model = model;
	return old;
}

Model* MeshRenderer::GetModel()
{
	return model;
}
