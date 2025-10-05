#include "Face.h"

Face::Face() {}

Face::Face(const std::vector<unsigned int>& vIndices, 
           const std::vector<unsigned int>& tIndices,
           const std::vector<unsigned int>& nIndices)
    : vertexIndices(vIndices), textureIndices(tIndices), normalIndices(nIndices) {}

std::vector<Face> Face::triangulate() const {
    std::vector<Face> triangles;
    
    if (vertexIndices.size() < 3) {
        return triangles;
    }
    
    if (vertexIndices.size() == 3) {
        triangles.push_back(*this);
        return triangles;
    }
    
    // Triangulação usando fan triangulation
    for (size_t i = 1; i < vertexIndices.size() - 1; i++) {
        std::vector<unsigned int> triVertices = {vertexIndices[0], vertexIndices[i], vertexIndices[i + 1]};
        std::vector<unsigned int> triTextures, triNormals;
        
        if (!textureIndices.empty()) {
            triTextures = {textureIndices[0], textureIndices[i], textureIndices[i + 1]};
        }
        
        if (!normalIndices.empty()) {
            triNormals = {normalIndices[0], normalIndices[i], normalIndices[i + 1]};
        }
        
        triangles.emplace_back(triVertices, triTextures, triNormals);
    }
    
    return triangles;
}