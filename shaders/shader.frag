#version 330 core

struct Material {
    vec3 diffuse;
    sampler2D diffuseMap;
    vec3 specular;
    sampler2D specularMap;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 attenuationCoefficients;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Material material;
uniform DirectionalLight light;
uniform PointLight pointLight;

vec3 CalculatePointLight(const in PointLight light, const vec3 diffuseColor, const vec3 specularColor, float shininess, const vec3 normal, const vec3 viewDir)
{
    float distance = length(light.position - FragPos);
    float constant = light.attenuationCoefficients.x;
    float linear = light.attenuationCoefficients.y;
    float quadratic = light.attenuationCoefficients.z;
    float attenuationDenominator = constant + linear * distance + quadratic * distance * distance;
    float attenuation = 1.0f / attenuationDenominator;

    vec3 ambient = diffuseColor * light.ambient;

    vec3 lightDir = normalize(light.position - FragPos);
    float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseAttenuation * diffuseColor * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * light.specular * specularColor;

    return (ambient + diffuse + specular) * attenuation;
}

void main()
{
    vec3 diffuseColor = material.diffuse * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specularColor = material.specular * vec3(texture(material.specularMap, TexCoords));
    vec3 ambient = diffuseColor * light.ambient;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseAttenuation * light.diffuse * diffuseColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * light.specular * specularColor;

    vec3 result = ambient + diffuse + specular;
    
    result += CalculatePointLight(pointLight, diffuseColor, specularColor, material.shininess, normal, viewDir);
    
    FragColor = vec4(result, 1);
}