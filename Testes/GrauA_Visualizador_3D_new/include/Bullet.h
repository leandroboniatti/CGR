#pragma once

#include <glm/glm.hpp>

class Bullet {
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 velocity;
    float speed;
    float lifetime;
    float maxLifetime;
    bool active;
    bool reflected;

public:
    Bullet();
    Bullet(const glm::vec3& startPos, const glm::vec3& dir, float bulletSpeed = 50.0f);
    ~Bullet();

    // Getters
    const glm::vec3& getPosition() const;
    const glm::vec3& getDirection() const;
    const glm::vec3& getVelocity() const;
    float getSpeed() const;
    float getLifetime() const;
    float getMaxLifetime() const;
    bool isActive() const;
    bool isReflected() const;

    // Setters
    void setPosition(const glm::vec3& pos);
    void setDirection(const glm::vec3& dir);
    void setSpeed(float bulletSpeed);
    void setMaxLifetime(float maxTime);
    void setActive(bool isActive);

    // Atualização
    void update(float deltaTime);

    // Colisão e reflexão
    void reflect(const glm::vec3& normal);
    void deactivate();

    // Utilitários
    glm::vec3 getNextPosition(float deltaTime) const;
};