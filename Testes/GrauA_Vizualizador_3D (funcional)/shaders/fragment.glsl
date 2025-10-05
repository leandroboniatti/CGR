#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool hasTexture;
uniform bool hasSpecularTexture;
uniform bool hasNormalTexture;

void main() {
    // Calcular normal (com suporte a normal mapping se disponível)
    vec3 norm = normalize(Normal);
    if (hasNormalTexture) {
        // Implementação simplificada de normal mapping
        vec3 normalMap = texture(normalTexture, TexCoord).rgb * 2.0 - 1.0;
        // Nota: Para normal mapping completo, seria necessário calcular TBN matrix
        norm = normalize(norm + normalMap * 0.1);
    }
    
    // Ambient
    vec3 ambient = material.ambient * lightColor;
    
    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * lightColor;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightColor;
    
    // Textures
    vec3 textureColor = vec3(1.0);
    if (hasTexture) {
        textureColor = texture(diffuseTexture, TexCoord).rgb;
    }
    
    vec3 specularColor = vec3(1.0);
    if (hasSpecularTexture) {
        specularColor = texture(specularTexture, TexCoord).rgb;
        specular *= specularColor;
    }
    
    // Emission
    vec3 emission = material.emission;
    
    // Combine all lighting components
    vec3 result = (ambient + diffuse + specular + emission) * textureColor;
    
    FragColor = vec4(result, 1.0);
}