#include "Group.h"
#include <glad/glad.h>
#include <iostream>

Group::Group() : name(""), materialName(""), VAO(0), VBO(0), EBO(0) {}

Group::Group(const std::string& groupName) 
    : name(groupName), materialName(""), VAO(0), VBO(0), EBO(0) {}

Group::~Group() {
    cleanup();
}

void Group::addFace(const Face& face) {
    // Triangula a face se necessário
    auto triangles = face.triangulate();
    for (const auto& triangle : triangles) {
        faces.push_back(triangle);
    }
}

void Group::generateVertexData(const std::vector<glm::vec3>& objVertices,
                              const std::vector<glm::vec2>& objTexCoords,
                              const std::vector<glm::vec3>& objNormals) {
    vertices.clear();
    indices.clear();
    
    unsigned int currentIndex = 0;
    
    for (const auto& face : faces) {
        for (size_t i = 0; i < face.vertexIndices.size(); i++) {
            // Posição
            if (face.vertexIndices[i] - 1 < objVertices.size()) {
                const auto& vertex = objVertices[face.vertexIndices[i] - 1];
                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.z);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            
            // Coordenadas de textura
            if (!face.textureIndices.empty() && i < face.textureIndices.size() && 
                face.textureIndices[i] - 1 < objTexCoords.size()) {
                const auto& texCoord = objTexCoords[face.textureIndices[i] - 1];
                vertices.push_back(texCoord.x);
                vertices.push_back(texCoord.y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            
            // Normais
            if (!face.normalIndices.empty() && i < face.normalIndices.size() && 
                face.normalIndices[i] - 1 < objNormals.size()) {
                const auto& normal = objNormals[face.normalIndices[i] - 1];
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
            
            indices.push_back(currentIndex++);
        }
    }
}

void Group::setupBuffers(const std::vector<glm::vec3>& objVertices,
                        const std::vector<glm::vec2>& objTexCoords,
                        const std::vector<glm::vec3>& objNormals) {
    generateVertexData(objVertices, objTexCoords, objNormals);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    std::cout << "Group \"" << name << "\" setup with " << faces.size() << " faces, " 
              << vertices.size() / 8 << " vertices" << std::endl;
}

void Group::render() const {
    if (VAO == 0) return;
    
    material.bind();
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    material.unbind();
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
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
}