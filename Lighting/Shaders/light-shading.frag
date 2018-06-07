#version 330 core

struct Material {
	sampler2D diffuse;
	vec3 specular;
	//sampler2D normal;
	float shininess;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	bool enabled;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	bool enabled;
};

uniform int num_p_lights;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;
uniform DirLight sun;
uniform bool apply_lighting;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

vec3 CalcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec4 texColor = texture(material.diffuse, TexCoords);
	if(texColor.a < 0.3)
	{
		discard;
	}
	if(!apply_lighting)
	{
		FragColor = vec4(vec3(texture(material.diffuse, TexCoords)), 1);
		return;
	}

	vec3 result;
	if(sun.enabled)
	{
		result = CalcDirectionalLight(sun, norm, viewDir);
	}

	for(int i = 0; i < num_p_lights; i++)
	{
		if(pointLights[i].enabled)
		{
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		}
	}

	//result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	
	FragColor = vec4(result, 1);
}

vec3 CalcDirectionalLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);

	//diffuse
	float diff = max(dot(normal, lightDir), 0);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0), material.shininess);

	//combine
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * material.specular;

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	//diffuse
	float diff = max(dot(normal, lightDir), 0);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0), material.shininess);

	//attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//combine
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * material.specular;

	//apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	//point light calculations
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0, 1);

	//diffuse
	float diff = max(dot(lightDir, normal), 0);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir), 0), material.shininess);

	//attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//combine
	vec3 ambient  = light.ambient * vec3(texture(material.diffuse, TexCoords)) * intensity;
	vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords)) * intensity;
	vec3 specular = light.specular * spec * material.specular, TexCoords;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}