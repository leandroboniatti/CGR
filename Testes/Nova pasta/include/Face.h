#pragma once

#include <vector>
#include <glm/glm.hpp>

class Face {
public:
    Face();
    ~Face();
    
    // Índices dos vértices, normais e coordenadas de textura
    std::vector<int> vertexIndices;
    std::vector<int> normalIndices;
    std::vector<int> textureIndices;
    
    // Material associado à face
    int materialIndex;
    
    // Métodos
    void addVertex(int vertexIndex, int textureIndex = -1, int normalIndex = -1);
    bool isTriangle() const;
    std::vector<Face> triangulate() const;
};