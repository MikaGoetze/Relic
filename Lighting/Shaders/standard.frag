#version 330 core

struct Material {
	sampler2D albedo;
	sampler2D normal;
	sampler2D metallic;
	sampler2D roughness;
	samplerCube irradiance_map;
	samplerCube prefilter_map;
	sampler2D brdf_lut;
	float shininess;
};

struct DirLight {
	sampler2D depth_map;
	vec3 direction;
	vec3 color;
	float intensity;
	mat4 light_space;
};

struct PointLight {
	samplerCube depth_map;
	vec3 position;
	float far;

	vec3 color;
	float intensity;
};

struct SpotLight {
	sampler2D depth_map;
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 color;
	float intensity;
	mat4 light_space;
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

float Shadow(sampler2D map, mat4 light_space, vec3 normal, vec3 lightDir)
{
	vec4 fp = light_space * vec4(FragPos, 1);
	//transform coords into 0,1
	vec3 proj_coords = fp.xyz / fp.w;
	proj_coords = proj_coords * 0.5 + 0.5;
	
	float closest_depth = texture(map, proj_coords.xy).r;
	float our_depth = proj_coords.z;
	
	float shadow_bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	
	float shad = 0.0;
	vec2 texel_size = 1.0 / textureSize(map, 0);
	float num_samples = 0;
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			num_samples += 1;
			float pcf_depth = texture(map, proj_coords.xy + vec2(x,y) * texel_size).r;
			shad += (our_depth - shadow_bias) > pcf_depth ? 0 : 1;
		}
	}
	
	shad = float(shad) / float(num_samples);
	
	if(proj_coords.z > 1)
		shad = 0;
	return shad;
}

float PointShadow(samplerCube map, vec3 lightPos, float far)
{
	vec3 lightDir = FragPos - lightPos;
	
	float our_depth = length(lightDir);
	
	float shadow = 0;
	float bias = 0.05;
	float samples = 2;
	float offset = 0.1;
	
	for(float x = -offset; x < offset; x += offset / (samples / 2))
	{
		for(float y = -offset; y < offset; y += offset / (samples / 2))
		{
			for(float z = -offset; z < offset; z += offset / (samples / 2))
			{
				float pcf_depth = texture(map, lightDir + vec3(x, y, z)).r * far;
				shadow += (our_depth - bias) > pcf_depth ? 0 : 1;
			}
		}
	}
	
	shadow = float(shadow) / float(pow(samples, 3));
	return shadow;
}

//PBR Calculation functions
vec3 FresnelShlick(vec3 H, vec3 V, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1 - max(dot(H, V), 0), 5);
}

float TrowbridgeReitzGGX(vec3 N, vec3 H, float roughness)
{
	return pow(roughness, 2) / (M_PI * pow((pow(max(dot(N, H), 0), 2) * (pow(roughness, 2) - 1) + 1), 2));
}

float GSchlickGGX(vec3 N, vec3 V, float roughness)
{
	float N_dot_V = max(dot(N, V), 0);
	float k = pow(roughness + 1, 2) / 8;
	
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
	vec3 F = FresnelShlick(H, viewDir, F0, roughness);
	
	vec3 spec = (NDF * G * F) / max(4 * max(dot(normal, viewDir), 0) * max(dot(normal, lightDir), 0), 0.001);
	
	vec3 kS = F;
	vec3 kD = (vec3(1) - kS) * (1 - roughness);
	
	return (kD * texture(material.albedo, TexCoords).xyz / M_PI + spec) * radiance * max(dot(normal, lightDir), 0);
}

void main()
{
	vec3 albedo = pow(texture(material.albedo, TexCoords).rgb, vec3(2.2));
	float metallic = texture(material.metallic, TexCoords).r;
	float roughness = texture(material.roughness, TexCoords).r;
	
	vec3 normal = GetTangentsFromNormalMap();
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflection = reflect(-viewDir, normal);
	
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	
	//Directional Light
	vec3 luminosity = CalcDirLight(sun, normal, viewDir, F0);
	
	//Point Lights
	for(int i = 0; i < num_p_lights; i++)
	{
		luminosity += CalcPointLight(pointLights[i], normal, FragPos, viewDir, F0);
	}
	
	//SpotLight

	luminosity += CalcSpotLight(spotLight, normal, FragPos, viewDir, F0);

	//Final IBL calculations
	vec3 F = FresnelShlick(normal, viewDir, F0, roughness);
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;
	
	vec3 irradiance = texture(material.irradiance_map, normal).rgb;
	vec3 diffuse = irradiance * albedo;
	
	const float MAX_LOD_LEVEL = 4.0;
	vec3 prefiltered_color = textureLod(material.prefilter_map, reflection, roughness * MAX_LOD_LEVEL).rgb;
	vec2 brdf = texture(material.brdf_lut, vec2(max(dot(normal, viewDir), 0), roughness)).rg;
	vec3 specular = prefiltered_color * (F * brdf.x + brdf.y);
	
	vec3 ambient = (kD * diffuse + specular);
	
	vec3 lightDir = FragPos - pointLights[0].position;
	vec3 color = luminosity + ambient;
	//vec3 color = vec3(texture(pointLights[0].depth_map, normalize(lightDir)).r) / pointLights[0].far;

	color = color / (color + vec3(1));
	color = pow(color, vec3(1/2.2));
	FragColor = vec4(color, 1);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(-light.direction);

	return CalcBRDF(normal, viewDir, lightDir, light.color, F0, 0, light.intensity, false) * Shadow(light.depth_map, light.light_space, normal, lightDir);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float distance = length(light.position - fragPos);
	
	return CalcBRDF(normal, viewDir, lightDir, light.color, F0, distance, light.intensity, true); //* PointShadow(light.depth_map, light.position, light.far);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0)
{
	vec3 lightDir = normalize(light.position - fragPos);

	//point light calculations
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0, 1);

	float distance = length(light.position - fragPos);
	
	vec3 brdf = CalcBRDF(normal, viewDir, lightDir, light.color, F0, distance, light.intensity, true) * Shadow(light.depth_map, light.light_space, normal, lightDir);

	return brdf * intensity;
}