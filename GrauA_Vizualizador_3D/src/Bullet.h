#ifndef BULLET_H
#define BULLET_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

using namespace glm;

class Bullet {
public:
    vec3 position;
    vec3 direction;
    float speed;
    float lifetime;
    float maxLifetime;
    bool active;
    
    // Renderização
    unsigned int VAO, VBO;
    
    Bullet();
    Bullet(const vec3& startPos, const vec3& dir, float bulletSpeed = 5.0f, float maxLife = 5.0f);
    ~Bullet();
    
    void update(float deltaTime);
    void draw(const Shader& shader) const;
    bool isActive() const { return active && lifetime < maxLifetime; }
    void reflect(const glm::vec3& normal);
    void deactivate() { active = false; }
    
    // Representação do raio para colisão
    vec3 getRayOrigin() const { return position; }
    vec3 getRayDirection() const { return normalize(direction); }

private:
    void setupMesh();
    void cleanup();
};

#endif