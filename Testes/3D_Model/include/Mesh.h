#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct MaterialInfo {
    glm::vec3 Ka{0.1f};
    glm::vec3 Kd{1.0f};
    glm::vec3 Ks{0.0f};
    float Ns = 32.0f;
    int diffuseTex = -1; // OpenGL texture id or -1
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
};

struct Mesh {
    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ebo = 0;
    size_t indexCount = 0;
    MaterialInfo material;
};
