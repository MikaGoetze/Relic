#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform bool rendermode;
uniform vec3 diffuse;

void main()
{
	vec3 norm = normalize(Normal);

	if(rendermode)
	{
		FragColor = vec4(diffuse, 1);
	}
	else
	{
		FragColor = vec4(Normal, 1);
	}
}

