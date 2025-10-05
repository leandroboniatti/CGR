#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture_diffuse1;
uniform vec3 lightDir;

void main() {
    // Ambient
    vec3 ambient = 0.2 * texture(texture_diffuse1, TexCoord).rgb;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * texture(texture_diffuse1, TexCoord).rgb;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}