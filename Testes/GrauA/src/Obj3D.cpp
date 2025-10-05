#include "Obj3D.h"

glm::vec3 Obj3D::getWorldMin() const {
    if (!mesh) return glm::vec3(0.0f);
    glm::vec4 worldMin = transform * glm::vec4(mesh->min, 1.0f);
    return glm::vec3(worldMin);
}

glm::vec3 Obj3D::getWorldMax() const {
    if (!mesh) return glm::vec3(0.0f);
    glm::vec4 worldMax = transform * glm::vec4(mesh->max, 1.0f);
    return glm::vec3(worldMax);
}

void Obj3D::setPosition(const glm::vec3& pos) {
    // Simple position setter - create new transform with position
    // For more complex transformations, you would need to preserve scale/rotation
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, pos);
}

void Obj3D::setRotation(const glm::vec3& rot) {
    // Simple rotation setter - create new transform with rotation
    transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, glm::radians(rot.x), glm::vec3(1,0,0));
    transform = glm::rotate(transform, glm::radians(rot.y), glm::vec3(0,1,0));
    transform = glm::rotate(transform, glm::radians(rot.z), glm::vec3(0,0,1));
}

void Obj3D::setScale(const glm::vec3& newScale) {
    // Simple scale setter - create new transform with scale
    transform = glm::mat4(1.0f);
    transform = glm::scale(transform, newScale);
}