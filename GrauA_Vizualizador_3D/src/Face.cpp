#include "Face.h"

Face::Face() {}

Face::Face(const std::vector<unsigned int>& vIndices, 
           const std::vector<unsigned int>& tIndices,
           const std::vector<unsigned int>& nIndices)
    : vertexIndices(vIndices), textureIndices(tIndices), normalIndices(nIndices) {}

std::vector<Face> Face::triangulate() const {

    std::vector<Face> faces_triangulares = {};   // Vetor para armazenar as faces triangulares
    
    // se a face tem menos de 3 vértices, não é possível formar um triângulo
    if (vertexIndices.size()  < 3) { return faces_triangulares; }

    // Se a face já é um triângulo, retorna ela mesma
    if (vertexIndices.size() == 3) {
        faces_triangulares.push_back(*this);
        return faces_triangulares;
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
        
        faces_triangulares.emplace_back(triVertices, triTextures, triNormals);
    }

    return faces_triangulares;
}