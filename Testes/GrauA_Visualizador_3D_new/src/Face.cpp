#include "Face.h"

Face::Face() {
}

Face::~Face() {
}

void Face::addVertexIndex(int index) {
    vertexIndices.push_back(index);
}

void Face::addTextureIndex(int index) {
    textureIndices.push_back(index);
}

void Face::addNormalIndex(int index) {
    normalIndices.push_back(index);
}

const std::vector<int>& Face::getVertexIndices() const {
    return vertexIndices;
}

const std::vector<int>& Face::getTextureIndices() const {
    return textureIndices;
}

const std::vector<int>& Face::getNormalIndices() const {
    return normalIndices;
}

std::vector<Face> Face::triangulate() const {
    std::vector<Face> triangles;
    
    if (vertexIndices.size() < 3) {
        return triangles; // Face inválida
    }
    
    if (vertexIndices.size() == 3) {
        triangles.push_back(*this); // Já é um triângulo
        return triangles;
    }
    
    // Triangulação em leque (fan triangulation)
    for (size_t i = 1; i < vertexIndices.size() - 1; ++i) {
        Face triangle;
        
        // Adiciona vértices
        triangle.addVertexIndex(vertexIndices[0]);
        triangle.addVertexIndex(vertexIndices[i]);
        triangle.addVertexIndex(vertexIndices[i + 1]);
        
        // Adiciona coordenadas de textura se existirem
        if (!textureIndices.empty() && textureIndices.size() == vertexIndices.size()) {
            triangle.addTextureIndex(textureIndices[0]);
            triangle.addTextureIndex(textureIndices[i]);
            triangle.addTextureIndex(textureIndices[i + 1]);
        }
        
        // Adiciona normais se existirem
        if (!normalIndices.empty() && normalIndices.size() == vertexIndices.size()) {
            triangle.addNormalIndex(normalIndices[0]);
            triangle.addNormalIndex(normalIndices[i]);
            triangle.addNormalIndex(normalIndices[i + 1]);
        }
        
        triangles.push_back(triangle);
    }
    
    return triangles;
}

bool Face::isTriangle() const {
    return vertexIndices.size() == 3;
}

size_t Face::getVertexCount() const {
    return vertexIndices.size();
}