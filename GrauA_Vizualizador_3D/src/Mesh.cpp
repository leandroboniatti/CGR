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

bool Mesh::loadFromOBJ(const std::string& path) {
    cleanup();

    // Carrega dados do OBJ chamando loadOBJ, método estático da classe OBJReader.
    // path - caminho do arquivo
    // vertices - vetor com os vértices do modelo no formato VEC3
    // texCoords - vetor com as coordenadas de textura no formato VEC2
    // normals - vetor com as normais de cada face no formato VEC3
    // groups - grupo de grupos - vetor com os grupos
    // materials - mapa de materiais
    if (!OBJReader::loadOBJ(path, vertices, texCoords, normals, groups, materials)) {
        return false;
    }
    
    // Gerar normais se não existirem
    if (normals.empty()) {
        calculateNormals();
    }
    
    calculateBoundingBox();
    setupBuffers();
    
    return true;
}

void Mesh::setupBuffers() {
    for (auto& group : groups) {
        group.setupBuffers(vertices, texCoords, normals);
    }
    
    std::cout << "Mesh buffers setup complete" << std::endl;
}

void Mesh::render(const Shader& shader) const {
    for (const auto& group : groups) {
        // Set material uniforms
        shader.setVec3("material.ambient", group.material.ambient);
        shader.setVec3("material.diffuse", group.material.diffuse);
        shader.setVec3("material.specular", group.material.specular);
        shader.setFloat("material.shininess", group.material.shininess);
        
        // Set texture uniforms
        shader.setInt("material.diffuseMap", 0);
        shader.setInt("material.specularMap", 1);
        shader.setInt("material.normalMap", 2);
        
        shader.setBool("material.hasDiffuseMap", group.material.diffuseTextureID != 0);
        shader.setBool("material.hasSpecularMap", group.material.specularTextureID != 0);
        shader.setBool("material.hasNormalMap", group.material.normalTextureID != 0);
        
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
    materials.clear();
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