#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform float time;
uniform sampler2D baseMap;



void main() 
{
	float p = 0.5 + (sin(time * 4) + 1) * 0.5f; // [0.5; 1.5]
	FragColor = pow(texture(baseMap, texCoord), vec4(p));
}