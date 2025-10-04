#version 450 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
} fs_in;

struct Material { vec3 Ka; vec3 Kd; vec3 Ks; float Ns; int hasDiffuseTex; };
uniform Material material;
uniform sampler2D diffuseTex;

struct DirLight { vec3 direction; vec3 ambient; vec3 diffuse; vec3 specular; };
uniform DirLight dirLight;

struct PointLight { bool enabled; vec3 position; vec3 ambient; vec3 diffuse; vec3 specular; float constant; float linear; float quadratic; };
uniform PointLight pointLight;

uniform vec3 viewPos;

vec3 applyDirLight(vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-dirLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    vec3 ambient = dirLight.ambient * material.Ka;
    vec3 col = material.Kd;
    if(material.hasDiffuseTex==1) col *= texture(diffuseTex, fs_in.Tex).rgb;
    vec3 diffuse = dirLight.diffuse * diff * col;
    vec3 specular = dirLight.specular * spec * material.Ks;
    return ambient + diffuse + specular;
}

vec3 applyPointLight(vec3 normal, vec3 viewDir) {
    if(!pointLight.enabled) return vec3(0.0);
    vec3 lightDir = normalize(pointLight.position - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    float dist = length(pointLight.position - fs_in.FragPos);
    float att = 1.0 / (pointLight.constant + pointLight.linear*dist + pointLight.quadratic*dist*dist);
    vec3 col = material.Kd;
    if(material.hasDiffuseTex==1) col *= texture(diffuseTex, fs_in.Tex).rgb;
    vec3 ambient = pointLight.ambient * material.Ka;
    vec3 diffuse = pointLight.diffuse * diff * col;
    vec3 specular = pointLight.specular * spec * material.Ks;
    return (ambient + diffuse + specular) * att;
}

void main(){
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 result = applyDirLight(norm, viewDir) + applyPointLight(norm, viewDir);
    FragColor = vec4(result,1.0);
}
