#version 410 core

out vec4 FragColor;

uniform vec3 bulletColor;

void main() {
    FragColor = vec4(bulletColor, 1.0);
}