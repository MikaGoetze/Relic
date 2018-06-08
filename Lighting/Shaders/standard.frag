#version 330 core

struct Material {
	sampler2D albedo;
	sampler2D normal;
	sampler2D metallic;
	sampler2D roughness;
	sampler2D ao;
	float shininess;
};

struct DirLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 color;
	float intensity;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 color;
	float intensity;
};

#define M_PI 3.1415926535897932384626433832795

uniform DirLight sun;
uniform int num_p_lights;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 F0);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0);

//Technique inspired by Joey De Vries

vec3 GetTangentsFromNormalMap()
{
	vec3 tangent_normal = texture(material.normal, TexCoords).xyz * 2.0 - 1.0;
	
	vec3 Q1 = dFdx(FragPos);
	vec3 Q2 = dFdy(FragPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);
	
	vec3 N = normalize(Normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);
	
	return normalize(TBN * tangent_normal);
}

void main()
{
	vec3 norm = GetTangentsFromNormalMap();

	vec3 viewDir = normalize(viewPos - FragPos);
	
	//calculate luminosity for this fragment
	
	vec3 F0 = vec3(0.04);
	vec3 albedo = pow(texture(material.albedo, TexCoords).rgb, vec3(2.2));
	float metallic = texture(material.metallic, TexCoords).r;
	
	F0 = mix(F0, albedo, metallic);
	
	//Directional Light
	vec3 luminosity = CalcDirLight(sun, norm, viewDir, F0);
	
	//Point Lights
	for(int i = 0; i < num_p_lights; i++)
	{
		luminosity += CalcPointLight(pointLights[i], norm, FragPos, viewDir, F0);
	}
	
	//SpotLight

	luminosity += CalcSpotLight(spotLight, norm, FragPos, viewDir, F0);

	vec3 color = vec3(0.03) * albedo + luminosity;
	color = color/(color + vec3(1));
	color = pow(color, vec3(1/2.2));
	
	FragColor = vec4(color, 1);
}

//PBR Calculation functions
vec3 FresnelShlick(vec3 H, vec3 V, vec3 F0)
{
	return F0 + (1 - F0) * pow(1 - max(dot(H, V), 0), 5);
}

float TrowbridgeReitzGGX(vec3 N, vec3 H, float roughness)
{
	return pow(roughness, 2) / (M_PI * pow((pow(max(dot(N, H), 0), 2) * (pow(roughness, 2) - 1) + 1), 2));
}

float GSchlickGGX(vec3 N, vec3 V, float roughness)
{
	float N_dot_V = max(dot(N, V), 0);
	float r = (roughness + 1);
	float k = (r * r) / 8;
	
	return N_dot_V / (N_dot_V * (1 - k) + k);
}

float GSmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	return GSchlickGGX(N, V, roughness) * GSchlickGGX(N, L, roughness);
}

vec3 CalcBRDF(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 lightColor, vec3 F0, float distance, float intensity, bool attenuate)
{
	float roughness = texture(material.roughness, TexCoords).r;
	
	vec3 H = normalize(viewDir + lightDir);
	
	//Apply attenuation (optionally)

	vec3 radiance = intensity * lightColor ;
	
	if(attenuate)
	{
		radiance *= 1/pow(distance, 2);
	}
	
	float NDF = TrowbridgeReitzGGX(normal, H, roughness);
	float G = GSmith(normal, viewDir, lightDir, roughness);
	vec3 F = FresnelShlick(H, viewDir, F0);
	
	vec3 spec = (NDF * G * F) / max(4 * max(dot(normal, viewDir), 0) * max(dot(normal, lightDir), 0), 0.001);
	
	vec3 kS = F;
	vec3 kD = (vec3(1) - kS) * (1 - texture(material.metallic, TexCoords).r);
	
	return (kD * texture(material.albedo, TexCoords).xyz / M_PI + spec) * radiance * max(dot(normal, lightDir), 0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(-light.direction);

	return CalcBRDF(normal, viewDir, lightDir, light.color, F0, 0, light.intensity, false);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float distance = length(light.position - fragPos);
	
	return CalcBRDF(normal, viewDir, lightDir, light.color, F0, distance, light.intensity, true);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(light.position - fragPos);

	//point light calculations
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0, 1);

	float distance = length(light.position - fragPos);
	
	vec3 brdf = CalcBRDF(normal, viewDir, lightDir, light.color, F0, distance, light.intensity, true);

	return brdf * intensity;
}