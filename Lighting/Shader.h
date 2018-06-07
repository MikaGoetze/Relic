#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <string>
#include <glm/mat4x2.hpp>

//Shader class to make interacting with shaders easier
//Inspiration taken from Joey de Vries (www.learnopengl.com)
class Shader
{
private:
	std::string v_path, f_path;
public:
	unsigned int shader_id;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	void SetActive();
	void SetBool(const std::string &name, bool value) const;
	void SetInt(const std::string &name, int value) const;
	void SetFloat(const std::string &name, float value) const;
	void SetVec4(const std::string &name, float x, float y, float z, float w = 1) const;
	void SetVec3(const std::string &name, float x, float y, float z) const;
	void SetVec3(const std::string &name, glm::vec3 vector) const;

	std::string GetFragmentPath();
	std::string GetVertexPath();

	void SetMat4(const std::string &name, glm::mat4 mat) const;
	~Shader();
};

#endif

