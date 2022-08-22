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
    vec3 attenuationCoefficients;

    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec2 cutOff;

    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Material material;
uniform DirectionalLight light;

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calculateDirectionalLight(const in DirectionalLight light, const vec3 diffuseColor, const vec3 specularColor, const float shininess, const vec3 normal, const vec3 viewDir)
{
    vec3 lightDir = light.direction;
    float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseAttenuation * light.diffuse * diffuseColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * light.specular * specularColor;

    return diffuse + specular;
}

vec3 calculatePointLight(const in PointLight light, const vec3 diffuseColor, const vec3 specularColor, const float shininess, const vec3 normal, const vec3 viewDir)
{
    float distance = length(light.position - FragPos);
    float constant = light.attenuationCoefficients.x;
    float linear = light.attenuationCoefficients.y;
    float quadratic = light.attenuationCoefficients.z;
    float attenuationDenominator = constant + linear * distance + quadratic * distance * distance;
    float attenuation = 1.0f / attenuationDenominator;

    vec3 lightDir = normalize(light.position - FragPos);
    float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseAttenuation * diffuseColor * light.diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * light.specular * specularColor;

    return (diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(const in SpotLight light, const vec3 diffuseColor, const vec3 specularColor, const float shininess, const vec3 normal, const vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir, normalize(- light.direction));
    float epsilon = light.cutOff.x - light.cutOff.y;
    float intensity = clamp((theta - light.cutOff.y) / epsilon, 0.0, 1.0);

    float diffuseAttenuation = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseAttenuation * light.diffuse * diffuseColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * light.specular * specularColor;

    return (diffuse + specular) * intensity;
}

void main()
{
    vec3 diffuseColor = material.diffuse * vec3(texture(material.diffuseMap, TexCoords));
    vec3 specularColor = material.specular * vec3(texture(material.specularMap, TexCoords));
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f);
    result += calculateDirectionalLight(light, diffuseColor, specularColor, material.shininess, normal, viewDir);
    
    for (int i = 0; i < NR_POINT_LIGHTS; i++) 
    {
        result += calculatePointLight(pointLights[i], diffuseColor, specularColor, material.shininess, normal, viewDir);
    }
    
    result += calculateSpotLight(spotLight, diffuseColor, specularColor, material.shininess, normal, viewDir);

    vec3 ambient = diffuseColor * light.ambient;
    result += ambient;

    FragColor = vec4(result, 1);
}