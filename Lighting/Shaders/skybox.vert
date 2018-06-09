#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 FragPos;

void main()
{
	FragPos = position;
	
	mat4 rotation_only_view = mat4(mat3(view));
	vec4 out_pos = projection * rotation_only_view * vec4(position, 1);

	//Force depth to always be 1 (maximum distance - behind everything)
	gl_Position = out_pos.xyww;
}