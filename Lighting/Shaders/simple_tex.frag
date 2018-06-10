#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depth_map;

void main()
{
	FragColor = vec4(vec3(texture(depth_map, TexCoords).r), 1);
}