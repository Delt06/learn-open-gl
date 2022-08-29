#version 330 core

in float Occlusion;

out vec4 FragColor;

void main()
{
    FragColor = vec4(0, 1 * Occlusion, 0, 1);
}