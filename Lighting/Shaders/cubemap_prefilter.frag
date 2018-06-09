#version 330 core

#define M_PI 3.14159265358979

in vec3 FragPos;

out vec4 FragColor;

uniform samplerCube environment_map;
uniform float roughness;

//Credit to Joey Van De Vries (www.leanopengl.com) for this function
float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

//Credit to Joey Van De Vries (www.leanopengl.com) for this function
vec2 Hammersley_Low_Discrep_Sample(uint i, uint N)
{
	return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 Importance_Sampler(vec2 Xi, vec3 N, float roughness)
{
	float a = pow(roughness, 2);
	
	float phi = 2 * M_PI * Xi.x;
	float cosTheta = sqrt((1 - Xi.y) / (1 + (pow(a, 2) - 1) * Xi.y));
	float sinTheta = sqrt(1 - pow(cosTheta, 2));
	
	//Transform for
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	vec3 up = abs(N.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}

float Distribution_Function(vec3 N, vec3 H, float roughness)
{
	return pow(roughness, 4) / (M_PI * pow(pow(max(dot(N, H), 0), 2) * (pow(roughness, 4) - 1) + 1, 2));
}

void main()
{
	vec3 normal = normalize(FragPos);
	vec3 R = normal;
	vec3 V = R;
	
	const uint sample_count = 1024u;
	float totalWeight = 0;
	vec3 prefilteredColour = vec3(0);
	
	for(uint i = 0u; i < sample_count; i++)
	{
		vec2 Xi = Hammersley_Low_Discrep_Sample(i, sample_count);
		vec3 H = Importance_Sampler(Xi, normal, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);
		
		float n_dot_l = max(dot(normal, L), 0);
		if(n_dot_l > 0)
		{
			float distribution = Distribution_Function(normal, H, roughness);
			float N_dot_H = max(dot(normal, H), 0);
			float H_dot_V = max(dot(H, V), 0);
			
			float pdf = distribution * N_dot_H / max((4 * H_dot_V), 0.0001);
			
			float saTexel = 4 * M_PI / (6 * pow(512, 2));
			float saSample = 1 / max(float(sample_count) * pdf, 0.0001);
			
			float mip_level = roughness == 0 ? 0 : 0.5 * log2(saSample / saTexel);
			
			prefilteredColour += textureLod(environment_map, L, mip_level).rgb * n_dot_l;
			totalWeight += n_dot_l;
		}
	}
	
	prefilteredColour = prefilteredColour / totalWeight;
	
	FragColor = vec4(prefilteredColour, 1);
}