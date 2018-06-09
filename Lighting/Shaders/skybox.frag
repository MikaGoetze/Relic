#version 330 core

in vec3 FragPos;

out vec4 FragColor;

uniform samplerCube environment_map;

void main()
{
	vec3 environment_color = texture(environment_map, FragPos).rgb;
	
	//Gamma correction
	environment_color = environment_color / (environment_color + vec3(1));
	environment_color = pow(environment_color, vec3(1/2.2));
	
	FragColor = vec4(environment_color, 1);
}