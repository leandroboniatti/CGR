#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

class Obj3D {
public:
    Mesh* mesh = nullptr;
    glm::mat4 transform{1.0f};
    int status = 0;  // 0=normal, 1=eliminável
    bool eliminavel = false;
    glm::vec3 direcao{0.0f, 0.0f, -1.0f};
    
    // Bounding box in world space
    glm::vec3 getWorldMin() const;
    glm::vec3 getWorldMax() const;
    
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scale);
};