#include "Obj3D.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Obj3D::Obj3D() : name(""), mesh(nullptr), transform(1.0f), position(0.0f), 
                 rotation(0.0f), scale(1.0f), eliminable(true), visible(true), collisionRadius(1.0f) {
}

Obj3D::~Obj3D() {
    cleanup();
}

void Obj3D::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransform();
}

void Obj3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateTransform();
}

void Obj3D::setScale(const glm::vec3& scl) {
    scale = scl;
    updateTransform();
}

void Obj3D::translate(const glm::vec3& translation) {
    position += translation;
    updateTransform();
}

void Obj3D::rotate(const glm::vec3& rot) {
    rotation += rot;
    updateTransform();
}

void Obj3D::scaleBy(const glm::vec3& scl) {
    scale *= scl;
    updateTransform();
}

void Obj3D::updateTransform() {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
}

void Obj3D::render(unsigned int shaderProgram) {
    if (!visible || !mesh) return;
    
    glUseProgram(shaderProgram);
    
    // Enviar matriz de transformação para o shader
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc != -1) {
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(transform));
    }
    
    mesh->render();
}

bool Obj3D::intersects(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    if (!mesh) return false;
    
    // Transformar o raio para o espaço local do objeto
    glm::mat4 invTransform = glm::inverse(transform);
    glm::vec4 localOrigin = invTransform * glm::vec4(rayOrigin, 1.0f);
    glm::vec4 localDirection = invTransform * glm::vec4(rayDirection, 0.0f);
    
    return mesh->intersectsBoundingBox(glm::vec3(localOrigin), glm::vec3(localDirection));
}

bool Obj3D::intersectsWithDistance(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance, glm::vec3& normal) const {
    if (!mesh) return false;
    
    // Usar esfera de colisão para detecção mais rápida
    glm::vec3 worldPos = getWorldPosition();
    glm::vec3 toObject = worldPos - rayOrigin;
    float projectionLength = glm::dot(toObject, rayDirection);
    
    // Se a projeção é negativa, o objeto está atrás do raio
    if (projectionLength < 0.0f) return false;
    
    glm::vec3 closestPoint = rayOrigin + rayDirection * projectionLength;
    float distanceToLine = glm::length(worldPos - closestPoint);
    
    // Usar o maior valor entre a escala para determinar o raio efetivo
    float effectiveRadius = collisionRadius * std::max(scale.x, std::max(scale.y, scale.z));
    
    if (distanceToLine <= effectiveRadius) {
        distance = projectionLength - sqrt(effectiveRadius * effectiveRadius - distanceToLine * distanceToLine);
        normal = glm::normalize(rayOrigin + rayDirection * distance - worldPos);
        return true;
    }
    
    return false;
}

glm::vec3 Obj3D::getBoundingBoxSize() const {
    if (!mesh) return glm::vec3(1.0f);
    
    glm::vec3 size = (mesh->maxBounds - mesh->minBounds) * scale;
    return size;
}

void Obj3D::calculateCollisionRadius() {
    if (!mesh) {
        collisionRadius = 1.0f;
        return;
    }
    
    glm::vec3 size = mesh->maxBounds - mesh->minBounds;
    collisionRadius = glm::length(size) * 0.5f;
}

glm::vec3 Obj3D::getWorldPosition() const {
    return glm::vec3(transform[3]);
}

void Obj3D::cleanup() {
    if (mesh) {
        mesh->cleanup();
        delete mesh;
        mesh = nullptr;
    }
}