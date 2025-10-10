#include "Group.h"
#include <glad/glad.h>
#include <iostream>

Group::Group()
    : name(""), VAO(0), VBO(0), vertexCount(0) {}

Group::Group(const string& groupName) 
    : name(groupName), VAO(0), VBO(0), vertexCount(0) {}

Group::~Group() { cleanup(); }

// Adiciona uma face ao grupo (vetor de faces), triangulando se necessário
void Group::addFace(const Face& face) {

    // Antes de adicionar a face do objeto ao grupo, "triangula" a face
    // dividindo ela em triângulos, usando "fan triangulation - ver Face.cpp"
    auto triangles = face.triangulate();

    for (const auto& triangle : triangles) {    // adiciona cada triângulo
        faces.push_back(triangle);              // ao grupo (vetor de faces)
    }
}


// Configura os buffers de OpenGL (VBO e VAO) para o grupo em processamento
void Group::setupBuffers(const vector<glm::vec3>& objVertices,      // recebe referência dos vetores que guardam a posição,
                         const vector<glm::vec2>& objTexCoords,     // textura e normais do objeto/Grupo em processamento,
                         const vector<glm::vec3>& objNormals   ) {  // acessados através dos índices das faces do grupo

    // Primeiro gera os dados sequenciais dos vértices do grupo, dentro do vetor "vertices"

    vertices.clear();   // limpa dados anteriores, se houver, do vetor que guardará as informações
                        // dos vértices a serem enviados para renderização. Inseridos sequencialmente.
                        // posição<3> + texCoord<2> + normal<3> = 8 floats por vértice
    
    for (const auto& face : faces) { // para cada face do grupo faz uma iteração e guarda informações em "vertices"

        for (size_t i = 0; i < face.vertexIndices.size(); i++) { // para cada posição de "vertexIndices" faz uma iteração
            
            if (face.vertexIndices[i] - 1 < objVertices.size()) {               // ajuste de índice (OBJ inicia em 1 e vector em 0)
                const auto& vertex = objVertices[face.vertexIndices[i] - 1];    // acessa a informação da posição do vértice indiretamente, via índice
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
                const auto& texCoord = objTexCoords[face.textureIndices[i] - 1];  // acessa a informação da coordenada de textura do vértice indiretamente, via índice
                vertices.push_back(texCoord.x);
                vertices.push_back(texCoord.y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            
            // Normais
            if (!face.normalIndices.empty() && i < face.normalIndices.size() && 
                face.normalIndices[i] - 1 < objNormals.size()) {
                const auto& normal = objNormals[face.normalIndices[i] - 1];  // acessa a informação da normal do vértice indiretamente, via índice
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);
            }
        }
    }
    
    // Calcular número de vértices do grupo
    vertexCount = vertices.size() / 8; // 8 floats por vértice (posição<3> + texCoord<2> + normal<3>)    


    // Agora sim, configura os buffers OpenGL (VBO e VAO) para o grupo

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);  // optamos por usar um único VBO para posições, texturas e normais
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
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

    //cout << "Grupo \"" << name << "\" configurado com " << faces.size() << " faces, "
    //     << vertexCount << " vertices" << endl; // cada vértice tem 8 floats (posição <3>, texCoord <2>, normal <3>)
}

void Group::render() const {

    if (VAO == 0) return;
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
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
}