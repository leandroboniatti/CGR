#include "Group.h"
#include "Common.h"

Group::Group() 
    : name("default")
    , VAO(0)
    , VBO(0)
    , EBO(0)
    , isSetup(false) {
}

Group::Group(const std::string& groupName)
    : name(groupName)
    , VAO(0)
    , VBO(0)
    , EBO(0)
    , isSetup(false) {
}

Group::~Group() {
    cleanup();
}

const std::string& Group::getName() const {
    return name;
}

const std::vector<Face>& Group::getFaces() const {
    return faces;
}

std::shared_ptr<Material> Group::getMaterial() const {
    return material;
}

unsigned int Group::getVAO() const {
    return VAO;
}

size_t Group::getIndexCount() const {
    return indices.size();
}

void Group::setName(const std::string& groupName) {
    name = groupName;
}

void Group::setMaterial(std::shared_ptr<Material> mat) {
    material = mat;
}

void Group::addFace(const Face& face) {
    // Triangula a face antes de adicionar
    std::vector<Face> triangles = face.triangulate();
    for (const auto& triangle : triangles) {
        faces.push_back(triangle);
    }
}

void Group::clearFaces() {
    faces.clear();
}

void Group::setupVAO(const std::vector<glm::vec3>& vertices,
                     const std::vector<glm::vec2>& texCoords,
                     const std::vector<glm::vec3>& normals) {
    if (faces.empty()) return;

    buildVertexData(vertices, texCoords, normals);

    // Gera objetos OpenGL
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Buffer de vértices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Buffer de índices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Posição (layout 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Coordenadas de textura (layout 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normais (layout 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    isSetup = true;
}

void Group::render() {
    if (!isSetup || VAO == 0) return;

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
    if (EBO != 0) {
        glDeleteBuffers(1, &EBO);
        EBO = 0;
    }
    isSetup = false;
}

void Group::buildVertexData(const std::vector<glm::vec3>& vertices,
                           const std::vector<glm::vec2>& texCoords,
                           const std::vector<glm::vec3>& normals) {
    vertexData.clear();
    indices.clear();

    // Mapeia cada combinação única de índices para um novo índice de vértice
    std::map<std::tuple<int, int, int>, unsigned int> vertexMap;
    unsigned int currentIndex = 0;

    for (const auto& face : faces) {
        const auto& vertIdx = face.getVertexIndices();
        const auto& texIdx = face.getTextureIndices();
        const auto& normIdx = face.getNormalIndices();

        for (size_t i = 0; i < vertIdx.size(); ++i) {
            int vIdx = vertIdx[i] - 1; // OBJ usa índices baseados em 1
            int tIdx = (i < texIdx.size()) ? texIdx[i] - 1 : -1;
            int nIdx = (i < normIdx.size()) ? normIdx[i] - 1 : -1;

            auto key = std::make_tuple(vIdx, tIdx, nIdx);
            
            auto it = vertexMap.find(key);
            if (it != vertexMap.end()) {
                // Vértice já existe, reutiliza o índice
                indices.push_back(it->second);
            } else {
                // Novo vértice, adiciona aos dados
                vertexMap[key] = currentIndex;
                indices.push_back(currentIndex);

                // Posição
                if (vIdx >= 0 && vIdx < vertices.size()) {
                    vertexData.push_back(vertices[vIdx].x);
                    vertexData.push_back(vertices[vIdx].y);
                    vertexData.push_back(vertices[vIdx].z);
                } else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }

                // Coordenadas de textura
                if (tIdx >= 0 && tIdx < texCoords.size()) {
                    vertexData.push_back(texCoords[tIdx].x);
                    vertexData.push_back(texCoords[tIdx].y);
                } else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(0.0f);
                }

                // Normais
                if (nIdx >= 0 && nIdx < normals.size()) {
                    vertexData.push_back(normals[nIdx].x);
                    vertexData.push_back(normals[nIdx].y);
                    vertexData.push_back(normals[nIdx].z);
                } else {
                    vertexData.push_back(0.0f);
                    vertexData.push_back(1.0f);
                    vertexData.push_back(0.0f);
                }

                currentIndex++;
            }
        }
    }
}