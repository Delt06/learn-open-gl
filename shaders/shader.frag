#version 330 core

struct Material {
	vec3 diffuse;
	sampler2D diffuseMap;
	vec3 specular;
	sampler2D specularMap;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

void main() 
{
	vec3 diffuseColor = material.diffuse * vec3(texture(material.diffuseMap, TexCoords));
	vec3 ambient = diffuseColor * light.ambient;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseAttenuation * light.diffuse * diffuseColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength  * material.specular * light.specular * vec3(texture(material.specularMap, TexCoords));;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1);
}