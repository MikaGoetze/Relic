#version 330 core

in vec2 TexCoords;

out vec2 FragColor;

#define M_PI 3.14159265358979

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

//PBR Calculation functions
vec3 FresnelShlick(vec3 H, vec3 V, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1 - roughness), F0) - F0) * pow(1 - max(dot(H, V), 0), 5);
}

float TrowbridgeReitzGGX(vec3 N, vec3 H, float roughness)
{
	return pow(roughness, 2) / (M_PI * pow((pow(max(dot(N, H), 0), 2) * (pow(roughness, 2) - 1) + 1), 2));
}

float GSchlickGGX(vec3 N, vec3 V, float roughness)
{
	float N_dot_V = max(dot(N, V), 0);
	float k = pow(roughness, 2) / 2;
	
	return N_dot_V / (N_dot_V * (1 - k) + k);
}

float GSmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	return GSchlickGGX(N, V, roughness) * GSchlickGGX(N, L, roughness);
}

//Credit to Joey Van De Vries (www.leanopengl.com) for logic of this function
vec2 Integrate_BRDF(float N_dot_V, float roughness)
{
	return vec2(N_dot_V, roughness);
	vec3 V = vec3(sqrt(1 - pow(N_dot_V, 2)), 0, N_dot_V);
	
	float A = 0;
	float B = 0;
	
	vec3 N = vec3(0,0,1);
	
	const uint sample_count = 1024u;
	
	for(uint i = 0u; i < sample_count; i++)
	{
		vec2 Xi = Hammersley_Low_Discrep_Sample(i, sample_count);
		vec3 H = Importance_Sampler(Xi, N, roughness);
		vec3 L = normalize(2 * dot(V, H) * H - V);
		
		float N_dot_L = max(L.z, 0);
		float N_dot_H = max(H.z, 0);
		float V_dot_H = max(dot(V, H), 0);
		
		if(N_dot_L > 0)
		{
			float G = GSmith(N, V, L, roughness);
			float G_vis = (G * V_dot_H) / (N_dot_H * N_dot_V);
			float Fc = pow(1 - V_dot_H, 5);
			
			A += (1 - Fc) * G_vis;
			B += Fc * G_vis;
		}
	}
	
	A /= float(sample_count);
	B /= float(sample_count);
	return vec2(A, B);
}

void main()
{
	FragColor = Integrate_BRDF(TexCoords.x, TexCoords.y);
}