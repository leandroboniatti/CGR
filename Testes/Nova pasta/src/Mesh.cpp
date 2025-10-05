#include "Mesh.h"
#include <algorithm>
#include <limits>

Mesh::Mesh() : name(""), minBounds(std::numeric_limits<float>::max()),
               maxBounds(std::numeric_limits<float>::lowest()) {
}

Mesh::~Mesh() {
    cleanup();
}

void Mesh::addVertex(const glm::vec3& vertex) {
    vertices.push_back(vertex);
}

void Mesh::addTexCoord(const glm::vec2& texCoord) {
    texCoords.push_back(texCoord);
}

void Mesh::addNormal(const glm::vec3& normal) {
    normals.push_back(normal);
}

void Mesh::addGroup(const Group& group) {
    groups.push_back(group);
}

void Mesh::setupBuffers() {
    for (auto& group : groups) {
        group.setupBuffers(vertices, texCoords, normals);
    }
}

void Mesh::render() {
    for (auto& group : groups) {
        group.render();
    }
}

void Mesh::cleanup() {
    for (auto& group : groups) {
        group.cleanup();
    }
}

void Mesh::calculateBounds() {
    if (vertices.empty()) return;
    
    minBounds = vertices[0];
    maxBounds = vertices[0];
    
    for (const auto& vertex : vertices) {
        minBounds.x = std::min(minBounds.x, vertex.x);
        minBounds.y = std::min(minBounds.y, vertex.y);
        minBounds.z = std::min(minBounds.z, vertex.z);
        
        maxBounds.x = std::max(maxBounds.x, vertex.x);
        maxBounds.y = std::max(maxBounds.y, vertex.y);
        maxBounds.z = std::max(maxBounds.z, vertex.z);
    }
}

bool Mesh::intersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
    // Ray-AABB intersection usando o algoritmo slab
    glm::vec3 invDir = 1.0f / rayDirection;
    glm::vec3 t1 = (minBounds - rayOrigin) * invDir;
    glm::vec3 t2 = (maxBounds - rayOrigin) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNear = std::max({tMin.x, tMin.y, tMin.z});
    float tFar = std::min({tMax.x, tMax.y, tMax.z});
    
    return tNear <= tFar && tFar >= 0.0f;
}