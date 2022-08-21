#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	vec4 frag_pos = model * vec4(aPos, 1);
	FragPos = vec3(frag_pos);
	gl_Position = projection * view * frag_pos;
	Normal = vec3(model * vec4(aNormal, 0));
	TexCoords = aTexCoords;
}