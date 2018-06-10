#version 330 core

in vec4 FragPos;

uniform vec3 light_position;
uniform float clipping_plane_far;

void main()
{
	gl_FragDepth = length(FragPos.xyz - light_position) / clipping_plane_far;
}