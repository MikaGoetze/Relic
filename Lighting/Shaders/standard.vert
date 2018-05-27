#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0)));
	vec3 B = cross(N, T);
	TBN = mat3(T, B, N);

	gl_Position = projection * view * model * vec4(pos, 1);
	FragPos = vec3(model * vec4(pos, 1));
	TexCoords = texCoord;
}