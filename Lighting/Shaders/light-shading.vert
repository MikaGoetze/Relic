#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
	gl_Position = projection * view * model * vec4(pos, 1);
	FragPos = vec3(model * vec4(pos, 1));
	TexCoords = texCoord;
	Normal = mat3(transpose(inverse(model))) * normal;
}