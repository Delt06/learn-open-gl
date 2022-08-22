#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    float alphaClipThreshold;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;

void main()
{
    vec4 diffuseColor = texture(material.texture_diffuse1, TexCoords);
    if (diffuseColor.a < material.alphaClipThreshold) discard;
    
    FragColor = vec4(diffuseColor.rgb, 1.0f);
}