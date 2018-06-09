#version 330 core

out vec4 FragColor;

in vec3 FragPos;

uniform sampler2D environment_map;

const vec2 inverseArctan = vec2(0.1591, 0.3183);
vec2 SampleFromSphericalMap(vec3 in_vector)
{
	return vec2(atan(in_vector.z, in_vector.x), asin(in_vector.y)) * inverseArctan + 0.5;
}

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
	//We normalise in position since we want a unit cube here.
	vec2 uv_coords = SampleSphericalMap(normalize(FragPos));
 
	FragColor = vec4(texture(environment_map, uv_coords).rgb, 1);
}
