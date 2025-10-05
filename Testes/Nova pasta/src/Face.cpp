#include "Face.h"

Face::Face() : materialIndex(-1) {
}

Face::~Face() {
}

void Face::addVertex(int vertexIndex, int textureIndex, int normalIndex) {
    vertexIndices.push_back(vertexIndex);
    if (textureIndex >= 0) {
        textureIndices.push_back(textureIndex);
    }
    if (normalIndex >= 0) {
        normalIndices.push_back(normalIndex);
    }
}

bool Face::isTriangle() const {
    return vertexIndices.size() == 3;
}

std::vector<Face> Face::triangulate() const {
    std::vector<Face> triangles;
    
    if (isTriangle()) {
        triangles.push_back(*this);
        return triangles;
    }
    
    // Fan triangulation
    for (size_t i = 1; i < vertexIndices.size() - 1; ++i) {
        Face triangle;
        triangle.materialIndex = materialIndex;
        
        // Primeiro vértice
        triangle.addVertex(vertexIndices[0], 
                          textureIndices.empty() ? -1 : textureIndices[0],
                          normalIndices.empty() ? -1 : normalIndices[0]);
        
        // Vértice i
        triangle.addVertex(vertexIndices[i],
                          textureIndices.empty() ? -1 : textureIndices[i],
                          normalIndices.empty() ? -1 : normalIndices[i]);
        
        // Vértice i+1
        triangle.addVertex(vertexIndices[i + 1],
                          textureIndices.empty() ? -1 : textureIndices[i + 1],
                          normalIndices.empty() ? -1 : normalIndices[i + 1]);
        
        triangles.push_back(triangle);
    }
    
    return triangles;
}