#ifndef MESH_H
#define MESH_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Group.h"

using namespace std;

// Estrutura para Axis-Aligned Bounding Box (AABB)
struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    
    BoundingBox() : min(FLT_MAX), max(-FLT_MAX) {}
    
    void expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    glm::vec3 center() const { return (min + max) * 0.5f; }

    glm::vec3 size() const { return max - min; }

    float radius() const { return glm::length(size()) * 0.5f; }
};


class Mesh {
public:
    vector<glm::vec3> vertices;  // Vetor com os vértices do objeto 3D
    vector<glm::vec2> texCoords; // Vetor com as coordenadas de textura do objeto 3D
    vector<glm::vec3> normals;   // Vetor com as normais do objeto 3D
    vector<Group> groups;        // Grupos que compõem a malha do objeto 3D
    
    BoundingBox boundingBox;    // estrutura da bounding box do objeto 3D
    
    // Construtor
    Mesh();

    // Destrutor
    ~Mesh();
    
    // Carrega dados do OBJ chamando OBJReader::readFileOBJ, método da classe OBJReader.
    // Este, por sua vez, preenche os vetores e mapas passados por referência.
    bool readObjectModel(string& path);

    // Configura os buffers OpenGL (VBOs, VAOs) para cada grupo da malha
    void setupBuffers();
    
    // Renderiza a malha chamando render() de cada grupo
    void render(const class Shader& shader) const;

    // Limpa os dados da malha e libera recursos OpenGL
    void cleanup();
    
    // Calcula a bounding box do modelo/objeto
    void calculateBoundingBox();

    // Testa interseção do segmento (ray) com a bounding box (retorna true se houver interseção)
    bool rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                     float& distance) const;

    // Calcula a normal de uma face dada pelos três vértices (v0, v1, v2)
    glm::vec3 calculateFaceNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) const;
};

#endif