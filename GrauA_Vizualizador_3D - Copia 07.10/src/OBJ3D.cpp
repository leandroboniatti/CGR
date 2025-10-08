#include "OBJ3D.h"
#include <iostream>

OBJ3D::OBJ3D() 
    : transform(1.0f), 
      position(0.0f), 
      rotation(0.0f), 
      scale(1.0f), 
      eliminable(true), 
      name("") {
    updateTransform();
}

OBJ3D::OBJ3D(const string& objName)
    : transform(1.0f),
      position (0.0f),
      rotation (0.0f),
      scale    (1.0f),
      eliminable(true),
      name(objName) {
    updateTransform();
}

OBJ3D::~OBJ3D() {
    mesh.cleanup();
}

bool OBJ3D::readFile(string& path) {
    if (!mesh.readOBJ(path)) {
        cerr << "Falha ao carregar arquivo OBJ: " << path << endl;
        return false;
    }

    cout << "Arquivo OBJ3D \"" << name << "\" carregado com sucesso de: " << path << endl;
    return true;
}

void OBJ3D::render(const Shader& shader) const {
    shader.setMat4("model", transform);
    mesh.render(shader);
}

void OBJ3D::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransform();
}

void OBJ3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateTransform();
}

void OBJ3D::setScale(const glm::vec3& scl) {
    scale = scl;
    updateTransform();
}

void OBJ3D::setEliminable(bool canEliminate) {
    eliminable = canEliminate;
}

void OBJ3D::translate(const glm::vec3& offset) {
    position += offset;
    updateTransform();
}

void OBJ3D::rotate(const glm::vec3& angles) {
    rotation += angles;
    updateTransform();
}

void OBJ3D::scaleBy(const glm::vec3& factor) {
    scale *= factor;
    updateTransform();
}

void OBJ3D::updateTransform() {
    transform = glm::mat4(1.0f);
    
    // Apply transformations in order: Scale -> Rotate -> Translate
    transform = glm::translate(transform, position);
    
    // Apply rotations (Euler angles)
    transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    
    transform = glm::scale(transform, scale);
}

BoundingBox OBJ3D::getTransformedBoundingBox() const {
    BoundingBox transformedBB;
    
    // Transform all 8 corners of the bounding box
    glm::vec3 corners[8] = {
        mesh.boundingBox.min,
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.min.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.max.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.min.y, mesh.boundingBox.max.z),
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.max.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.min.y, mesh.boundingBox.max.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.max.y, mesh.boundingBox.max.z),
        mesh.boundingBox.max
    };
    
    for (int i = 0; i < 8; i++) {
        glm::vec4 transformedCorner = transform * glm::vec4(corners[i], 1.0f);
        transformedBB.expand(glm::vec3(transformedCorner));
    }
    
    return transformedBB;
}

bool OBJ3D::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    // Transform ray to object space
    glm::mat4 invTransform = glm::inverse(transform);
    glm::vec4 localOrigin = invTransform * glm::vec4(rayOrigin, 1.0f);
    glm::vec4 localDirection = invTransform * glm::vec4(rayDirection, 0.0f);
    
    return mesh.rayIntersect(glm::vec3(localOrigin), glm::normalize(glm::vec3(localDirection)), distance);
}