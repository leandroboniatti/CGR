#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <string>
#include "Face.h"

using namespace std;

class Group {
public:
    string name;
    vector<Face> faces;
    
    // OpenGL objects
    unsigned int VAO;
    unsigned int VBO;

    // Vetor de dados (floats) dos vértices (posições, normais, coordenadas de textura)
    // para envio à OpenGL. Armazena sequencialmente os atributos de cada vértice.
    // Exemplo: v1.x, v1.y, v1.z, v1.nx, v1.ny, v1.nz, v1.u, v1.v, v2.x, v2.y, ...
    // Cada grupo de 8 floats representa um vértice (posição<3> + texCoord<2> + normal<3>)
    vector<float> vertices;

    int vertexCount; // Número de vértices do grupo para envio à glDrawArrays
                     // cada vértice tem 8 floats (posição<3> + texCoord<2> + normal<3>)
                     // logo, vertexCount = vertices.size() / 8
    
    Group();

    Group(const string& groupName); // Construtor com nome do grupo

    ~Group();

    void addFace(const Face& face);

    // Configura os buffers de OpenGL (VBO e VAO) para o grupo em processamento
    // recebe referência dos vetores que guardam a posição, textura e normais do
    // objeto/Grupo em processamento,acessados através dos índices das faces do grupo
    void setupBuffers(const vector<glm::vec3>& objVertices,
                      const vector<glm::vec2>& objTexCoords,
                      const vector<glm::vec3>& objNormals);

    // Renderiza o grupo de faces
    void render() const;

    void cleanup();
};

#endif