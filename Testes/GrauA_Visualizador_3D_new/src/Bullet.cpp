#include "Bullet.h"

Bullet::Bullet()
    : position(0.0f)
    , direction(0.0f, 0.0f, -1.0f)
    , speed(50.0f)
    , lifetime(0.0f)
    , maxLifetime(5.0f)
    , active(false)
    , reflected(false) {
    velocity = direction * speed;
}

Bullet::Bullet(const glm::vec3& startPos, const glm::vec3& dir, float bulletSpeed)
    : position(startPos)
    , direction(glm::normalize(dir))
    , speed(bulletSpeed)
    , lifetime(0.0f)
    , maxLifetime(5.0f)
    , active(true)
    , reflected(false) {
    velocity = direction * speed;
}

Bullet::~Bullet() {
}

const glm::vec3& Bullet::getPosition() const {
    return position;
}

const glm::vec3& Bullet::getDirection() const {
    return direction;
}

const glm::vec3& Bullet::getVelocity() const {
    return velocity;
}

float Bullet::getSpeed() const {
    return speed;
}

float Bullet::getLifetime() const {
    return lifetime;
}

float Bullet::getMaxLifetime() const {
    return maxLifetime;
}

bool Bullet::isActive() const {
    return active;
}

bool Bullet::isReflected() const {
    return reflected;
}

void Bullet::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Bullet::setDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
    velocity = direction * speed;
}

void Bullet::setSpeed(float bulletSpeed) {
    speed = bulletSpeed;
    velocity = direction * speed;
}

void Bullet::setMaxLifetime(float maxTime) {
    maxLifetime = maxTime;
}

void Bullet::setActive(bool isActive) {
    active = isActive;
}

void Bullet::update(float deltaTime) {
    if (!active) return;
    
    lifetime += deltaTime;
    position += velocity * deltaTime;
    
    // Desativa o tiro se passou do tempo limite
    if (lifetime >= maxLifetime) {
        active = false;
    }
}

void Bullet::reflect(const glm::vec3& normal) {
    direction = glm::reflect(direction, glm::normalize(normal));
    velocity = direction * speed;
    reflected = true;
}

void Bullet::deactivate() {
    active = false;
}

glm::vec3 Bullet::getNextPosition(float deltaTime) const {
    return position + velocity * deltaTime;
}