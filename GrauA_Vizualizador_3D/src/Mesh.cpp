#include "Mesh.h"
#include "OBJReader.h"
#include "Shader.h"
#include <iostream>
#include <algorithm>
#include <cfloat>

Mesh::Mesh() {}

Mesh::~Mesh() {
    cleanup();
}

bool Mesh::meshReadFileOBJ(string& path) {
    // Carrega dados do OBJ chamando OBJReader::readFileOBJ, método da classe OBJReader.
    // Este, por sua vez, preenche os vetores e mapas passados por referência.
    // path - caminho do arquivo, recebido como parâmetro
    // vertices - vetor com os vértices do modelo, no formato VEC3, definido na classe Mesh
    // texCoords - vetor com as coordenadas de textura, no formato VEC2, definido na classe Mesh
    // normals - vetor com as normais de cada face no formato VEC3, definido na classe Mesh
    // groups - grupo de grupos - vetor com os grupos, definido na classe Mesh
    // map<string, Material> emptyMaterials; // Mapa vazio, não usaremos materiais
    if (!OBJReader::readFileOBJ(path, vertices, texCoords, normals, groups)) {
        return false;
    }
    
    // Gerar normais se não existirem
    //if (normals.empty()) { calculateNormals(); }
    
    // Calcula a bounding box do modelo/objeto
    calculateBoundingBox();

    
    return true;
}

// Configura buffers OpenGL (VBOs, VAOs) para cada grupo da malha
void Mesh::setupBuffers() {
    for (auto& group : groups) { group.setupBuffers(vertices, texCoords, normals);}
    
    cout << "Mesh buffers setup complete" << endl;
}

void Mesh::render(const Shader& shader) const {
    for (const auto& group : groups) {
        // Don't override texture uniforms set by OBJ3D
        group.render();
    }
}

void Mesh::cleanup() {
    for (auto& group : groups) {
        group.cleanup();
    }
    groups.clear();
    vertices.clear();
    texCoords.clear();
    normals.clear();
}

void Mesh::calculateBoundingBox() {
    boundingBox = BoundingBox();
    
    for (const auto& vertex : vertices) {
        boundingBox.expand(vertex);
    }
    
    //std::cout << "Bounding box calculated: min(" << boundingBox.min.x << ", " 
    //          << boundingBox.min.y << ", " << boundingBox.min.z << "), max(" 
    //         << boundingBox.max.x << ", " << boundingBox.max.y << ", " 
    //          << boundingBox.max.z << ")" << std::endl;
}

bool Mesh::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    // Ray-AABB intersection test
    glm::vec3 invDir = 1.0f / rayDirection;
    glm::vec3 t1 = (boundingBox.min - rayOrigin) * invDir;
    glm::vec3 t2 = (boundingBox.max - rayOrigin) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNear = glm::max(glm::max(tMin.x, tMin.y), tMin.z);
    float tFar = glm::min(glm::min(tMax.x, tMax.y), tMax.z);
    
    if (tNear > tFar || tFar < 0.0f) {
        return false;
    }
    
    distance = tNear > 0.0f ? tNear : tFar;
    return true;
}

void Mesh::calculateNormals() {
    normals.clear();
    normals.resize(vertices.size(), glm::vec3(0.0f));
    
    // Calculate face normals and accumulate vertex normals
    for (const auto& group : groups) {
        for (const auto& face : group.faces) {
            if (face.vertexIndices.size() >= 3) {
                // Get vertices of the triangle
                glm::vec3 v1 = vertices[face.vertexIndices[0] - 1];
                glm::vec3 v2 = vertices[face.vertexIndices[1] - 1];
                glm::vec3 v3 = vertices[face.vertexIndices[2] - 1];
                
                // Calculate face normal
                glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
                
                // Accumulate normal for each vertex
                for (unsigned int idx : face.vertexIndices) {
                    normals[idx - 1] += normal;
                }
            }
        }
    }
    
    // Normalize accumulated normals
    for (auto& normal : normals) {
        if (glm::length(normal) > 0.0f) {
            normal = glm::normalize(normal);
        } else {
            normal = glm::vec3(0.0f, 1.0f, 0.0f); // Default up vector
        }
    }
    
    //std::cout << "Generated " << normals.size() << " vertex normals" << std::endl;
}

bool Mesh::rayIntersectPrecise(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                              float& distance, glm::vec3& hitPoint, glm::vec3& hitNormal) const {
    // Primeiro verifica colisão com bounding box
    if (!rayIntersect(rayOrigin, rayDirection, distance)) {
        return false;
    }
    
    float closestDistance = FLT_MAX;
    glm::vec3 closestHitPoint;
    glm::vec3 closestNormal;
    bool hitFound = false;
    
    // Testa intersecção com todas as faces
    for (const auto& group : groups) {
        for (const auto& face : group.faces) {
            if (face.vertexIndices.size() >= 3) {
                // Pega os vértices da face (triângulo)
                glm::vec3 v0 = vertices[face.vertexIndices[0] - 1];
                glm::vec3 v1 = vertices[face.vertexIndices[1] - 1];
                glm::vec3 v2 = vertices[face.vertexIndices[2] - 1];
                
                float faceDistance;
                glm::vec3 faceHitPoint;
                
                if (rayTriangleIntersect(rayOrigin, rayDirection, v0, v1, v2, faceDistance, faceHitPoint)) {
                    if (faceDistance < closestDistance) {
                        closestDistance = faceDistance;
                        closestHitPoint = faceHitPoint;
                        closestNormal = calculateFaceNormal(v0, v1, v2);
                        hitFound = true;
                    }
                }
            }
        }
    }
    
    if (hitFound) {
        distance = closestDistance;
        hitPoint = closestHitPoint;
        hitNormal = closestNormal;
        return true;
    }
    
    return false;
}

bool Mesh::continuousRayIntersect(const glm::vec3& rayStart, const glm::vec3& rayEnd,
                                 float& distance, glm::vec3& hitPoint, glm::vec3& hitNormal) const {
    glm::vec3 direction = rayEnd - rayStart;
    float rayLength = glm::length(direction);
    
    if (rayLength == 0.0f) return false;
    
    direction = glm::normalize(direction);
    
    float intersectDistance;
    if (rayIntersectPrecise(rayStart, direction, intersectDistance, hitPoint, hitNormal)) {
        if (intersectDistance <= rayLength) {
            distance = intersectDistance;
            return true;
        }
    }
    
    return false;
}

bool Mesh::rayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                               const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                               float& distance, glm::vec3& hitPoint) const {
    const float EPSILON = 0.0000001f;
    
    glm::vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = glm::cross(rayDirection, edge2);
    a = glm::dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) {
        return false; // Ray is parallel to triangle
    }
    
    f = 1.0f / a;
    s = rayOrigin - v0;
    u = f * glm::dot(s, h);
    
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    
    q = glm::cross(s, edge1);
    v = f * glm::dot(rayDirection, q);
    
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    
    // Calculate where the intersection point is on the line
    float t = f * glm::dot(edge2, q);
    
    if (t > EPSILON) { // Ray intersection
        distance = t;
        hitPoint = rayOrigin + rayDirection * t;
        return true;
    }
    
    return false; // Line intersection but not ray intersection
}

glm::vec3 Mesh::calculateFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) const {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    return glm::normalize(glm::cross(edge1, edge2));
}