#version 450 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTex;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 Tex;
} vs_out;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main(){
    vec4 worldPos = uModel * vec4(aPos,1.0);
    vs_out.FragPos = worldPos.xyz;
    vs_out.Normal = mat3(transpose(inverse(uModel))) * aNormal;
    vs_out.Tex = aTex;
    gl_Position = uProj * uView * worldPos;
}
