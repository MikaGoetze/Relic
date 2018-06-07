#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/mat4x2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Core/Util.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	v_path = vertexPath;
	f_path = fragmentPath;

	std::string vertex_code;
	std::string fragment_code;
	std::ifstream vertex_shader_file;
	std::ifstream fragment_shader_file;

	//Lets load in the files
	vertex_shader_file.open(vertexPath);
	fragment_shader_file.open(fragmentPath);
	std::stringstream vertex_stream, fShaderStream;

	//lets read them
	vertex_stream << vertex_shader_file.rdbuf();
	fShaderStream << fragment_shader_file.rdbuf();

	//close them once were done with them
	vertex_shader_file.close();
	fragment_shader_file.close();
	
	//convert them to strings
	vertex_code = vertex_stream.str();
	fragment_code = fShaderStream.str();
	//convert them to c_strings
	const char* vertex_shader_character_array = vertex_code.c_str();
	const char* fragment_shader_character_array = fragment_code.c_str();

	//compile the shader
	unsigned int vertex, fragment;
	int success;
	char info_log[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_character_array, NULL);
	glCompileShader(vertex);

	//Lets make sure that it actually compiled, and otherwise print out the error
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		Util::Log("[Relic][Shader] Error : " + std::string(vertexPath) + " compilation failed.\n" + std::string(info_log));
	}

	//now we do the same for the fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader_character_array, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		Util::Log("[Relic][Shader] Error : " + std::string(fragmentPath) + " compilation failed\n" + std::string(info_log));
	}

	//lets rememeber the id, and create the shader program
	shader_id = glCreateProgram();
	glAttachShader(shader_id, vertex);
	glAttachShader(shader_id, fragment);
	glLinkProgram(shader_id);
	
	//again, lets make sure that everything happened correctly
	glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader_id, 512, NULL, info_log);
		Util::Log("[Relic][Shader] Error : linking failed\n" + std::string(info_log));
	}

	//we dont need these anymore so lets delete them
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::SetActive()
{
	glUseProgram(shader_id);
}

void Shader::SetBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(shader_id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(shader_id, name.c_str()), value);
}
 
void Shader::SetFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(shader_id, name.c_str()), value);
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(shader_id, name.c_str()), x, y, z, w);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shader_id, name.c_str()), x, y, z);
}

void Shader::SetVec3(const std::string& name, glm::vec3 vector) const
{
	glUniform3f(glGetUniformLocation(shader_id, name.c_str()), vector.x, vector.y, vector.z);
}

std::string Shader::GetFragmentPath()
{
	return f_path;
}

std::string Shader::GetVertexPath()
{
	return v_path;
}

void Shader::SetMat4(const std::string& name, glm::mat4 mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader_id, name.c_str()), 1, false, glm::value_ptr(mat));
}

Shader::~Shader()
{
}

