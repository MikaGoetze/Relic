#version 330 core

in vec3 FragPos;

out vec4 FragColor;

uniform samplerCube environment_map;

#define M_PI 3.14159265358979

void main()
{
	//radiance at any point is based on its position
	vec3 normal = normalize(FragPos);
	
	vec3 irradiance = vec3(0);
	
	vec3 right = cross(vec3(0,1,0), normal);
	vec3 up = cross(normal, right);
	
	float sample_step = 0.025;
	float num_samples = 0;
	
	for(float b = 0; b < 2 * M_PI; b += sample_step)
	{
		for(float a = 0; a < 0.5 * M_PI; a += sample_step)
		{
			vec3 tangent_sample = vec3(sin(a) * cos(b), sin(a) * sin(b), cos(a));
			
			vec3 sample = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal;
			
			irradiance += texture(environment_map, sample).rgb * cos(a) * sin(a);
			num_samples += 1;
		}
	}

	irradiance = M_PI * irradiance * (1/float(num_samples));
	
	FragColor = vec4(irradiance, 1);
	
}