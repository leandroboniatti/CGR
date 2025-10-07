#ifndef FACE_H
#define FACE_H

#include <vector>
#include <glm/glm.hpp>

class Face {
public:
    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> textureIndices;
    std::vector<unsigned int> normalIndices;
    
    Face();
    Face(const std::vector<unsigned int>& vIndices, 
         const std::vector<unsigned int>& tIndices = {},
         const std::vector<unsigned int>& nIndices = {});
    
    // Converte face com 4 ou mais vértices em triângulos
    std::vector<Face> triangulate() const;
    
    bool isTriangle() const { return vertexIndices.size() == 3; }
};

#endif