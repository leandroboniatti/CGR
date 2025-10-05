#version 400 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    
    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    
    bool hasDiffuseMap;
    bool hasSpecularMap;
    bool hasNormalMap;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform vec3 objectColor;

void main() {
    vec3 ambient = light.ambient * material.ambient;
    if (material.hasDiffuseMap) {
        ambient *= texture(material.diffuseMap, TexCoord).rgb;
    }
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    if (material.hasDiffuseMap) {
        diffuse *= texture(material.diffuseMap, TexCoord).rgb;
    }
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;
    if (material.hasSpecularMap) {
        specular *= texture(material.specularMap, TexCoord).rgb;
    }
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * objectColor, 1.0);
}