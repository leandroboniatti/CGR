#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"

class Obj3D {
public:
    Obj3D();
    ~Obj3D();
    
    // Propriedades
    std::string name;
    Mesh* mesh;
    glm::mat4 transform;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    // Propriedades do jogo
    bool eliminable;
    bool visible;
    float collisionRadius;
    
    // Métodos de transformação
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scl);
    void translate(const glm::vec3& translation);
    void rotate(const glm::vec3& rotation);
    void scaleBy(const glm::vec3& scale);
    void updateTransform();
    
    // Métodos de renderização
    void render(unsigned int shaderProgram);
    
    // Métodos de colisão
    bool intersects(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const;
    bool intersectsWithDistance(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance, glm::vec3& normal) const;
    glm::vec3 getWorldPosition() const;
    glm::vec3 getBoundingBoxSize() const;
    void calculateCollisionRadius();
    
    // Cleanup
    void cleanup();
};