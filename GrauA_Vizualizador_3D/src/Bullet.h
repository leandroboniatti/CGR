#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

class Bullet {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float lifetime;
    float maxLifetime;
    bool active;
    
    // Rendering
    unsigned int VAO, VBO;
    
    Bullet();
    Bullet(const glm::vec3& startPos, const glm::vec3& dir, float bulletSpeed = 50.0f, float maxLife = 5.0f);
    ~Bullet();
    
    void update(float deltaTime);
    void render(const Shader& shader) const;
    bool isActive() const { return active && lifetime < maxLifetime; }
    
    void reflect(const glm::vec3& normal);
    void deactivate() { active = false; }
    
    // Ray representation for collision
    glm::vec3 getRayOrigin() const { return position; }
    glm::vec3 getRayDirection() const { return glm::normalize(direction); }
    
private:
    void setupMesh();
    void cleanup();
};

#endif