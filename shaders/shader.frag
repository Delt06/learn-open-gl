#version 330 core

in vec4 vertexColor;
out vec4 FragColor;

uniform float time;

void main() 
{
	float p = 0.5 + (sin(time * 4) + 1) * 0.5f; // [0.5; 1.5]
	FragColor = pow(vertexColor, p.xxxx);
}