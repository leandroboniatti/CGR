#include "Group.h"
#include <glad/glad.h>
#include <iostream>

Group::Group() : name("default"), material(nullptr), VAO(0), VBO(0), numVertices(0) {
}

Group::Group(const std::string& name) : name(name), material(nullptr), VAO(0), VBO(0), numVertices(0) {
}

Group::~Group() {
    cleanup();
}

void Group::addFace(const Face& face) {
    faces.push_back(face);
}

void Group::setMaterial(Material* mat) {
    material = mat;
}

void Group::setupBuffers(const std::vector<glm::vec3>& vertices,
                        const std::vector<glm::vec2>& texCoords,
                        const std::vector<glm::vec3>& normals) {
    
    std::vector<float> vertexData;
    
    // Para cada face do grupo
    for (const auto& face : faces) {
        // Para cada vértice da face (assumindo triângulos)
        for (size_t i = 0; i < face.vertexIndices.size(); ++i) {
            int vIdx = face.vertexIndices[i];
            
            // Posição
            if (vIdx < vertices.size()) {
                vertexData.push_back(vertices[vIdx].x);
                vertexData.push_back(vertices[vIdx].y);
                vertexData.push_back(vertices[vIdx].z);
            }
            
            // Coordenadas de textura
            if (i < face.textureIndices.size() && face.textureIndices[i] < texCoords.size()) {
                vertexData.push_back(texCoords[face.textureIndices[i]].x);
                vertexData.push_back(texCoords[face.textureIndices[i]].y);
            } else {
                vertexData.push_back(0.0f);
                vertexData.push_back(0.0f);
            }
            
            // Normais
            if (i < face.normalIndices.size() && face.normalIndices[i] < normals.size()) {
                vertexData.push_back(normals[face.normalIndices[i]].x);
                vertexData.push_back(normals[face.normalIndices[i]].y);
                vertexData.push_back(normals[face.normalIndices[i]].z);
            } else {
                vertexData.push_back(0.0f);
                vertexData.push_back(1.0f);
                vertexData.push_back(0.0f);
            }
        }
    }
    
    numVertices = vertexData.size() / 8; // 3 pos + 2 tex + 3 norm
    
    generateBuffers();
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    
    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Coordenadas de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Normais
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Group::render() {
    if (VAO == 0) return;
    
    if (material) {
        material->bind();
    }
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);
    
    if (material) {
        material->unbind();
    }
}

void Group::cleanup() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}

void Group::generateBuffers() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
    }
    if (VBO == 0) {
        glGenBuffers(1, &VBO);
    }
}