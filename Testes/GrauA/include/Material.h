#pragma once
#include <string>
#include <glm/glm.hpp>

class Material {
public:
    std::string name;
    glm::vec3 Ka{0.2f, 0.2f, 0.2f};  // ambient
    glm::vec3 Kd{0.8f, 0.8f, 0.8f};  // diffuse
    glm::vec3 Ks{0.0f, 0.0f, 0.0f};  // specular
    float Ns = 1.0f;                 // shininess
    std::string mapKd;               // diffuse texture path
    
    // OpenGL texture ID
    unsigned int textureId = 0;
};