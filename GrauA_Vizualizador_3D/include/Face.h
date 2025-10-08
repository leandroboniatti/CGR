#ifndef FACE_H
#define FACE_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;

class Face {
public:
    vector<unsigned int> vertexIndices;
    vector<unsigned int> textureIndices;
    vector<unsigned int> normalIndices;
    
    Face();

    Face(const std::vector<unsigned int>& vIndices,         // SEM valor padrão - parâmetro obrigatório
         const std::vector<unsigned int>& tIndices = {},    // COM valor padrão - parâmetro opcional
         const std::vector<unsigned int>& nIndices = {});   // COM valor padrão - parâmetro opcional

    // Converte face com 4 ou mais vértices em triângulos
    // usando "fan triangulation"
    std::vector<Face> triangulate() const;
    
    bool isTriangle() const { return vertexIndices.size() == 3; }
};

#endif