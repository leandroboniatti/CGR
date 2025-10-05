#pragma once
#include <glm/glm.hpp>

class Bullet {
private:
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float lifetime;
    float maxLifetime;
    bool active;

public:
    Bullet(const glm::vec3& startPos, const glm::vec3& dir, float bulletSpeed = 50.0f, float maxLife = 5.0f);
    
    void update(float deltaTime);
    void render(unsigned int shaderProgram);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getDirection() const { return direction; }
    bool isActive() const { return active; }
    
    // Setters
    void setActive(bool value) { active = value; }
    void reflect(const glm::vec3& normal);
};