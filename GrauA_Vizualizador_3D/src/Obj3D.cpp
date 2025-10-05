#include "Obj3D.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Obj3D::Obj3D(const std::string& objFile, const glm::vec3& position, 
             const glm::vec3& rotation, const glm::vec3& scale, bool eliminable)
    : eliminable(eliminable), objPath(objFile), modelMatrix(1.0f) {

    setPosition(position);
    setRotation(rotation);
    setScale(scale);
    updateModelMatrix();
}

Obj3D::~Obj3D() {
    for (auto& group : groups) {
        if (group.VAO) glDeleteVertexArrays(1, &group.VAO);
        if (group.VBO) glDeleteBuffers(1, &group.VBO);
        if (group.EBO) glDeleteBuffers(1, &group.EBO);
    }
    
    for (auto& material : materials) {
        if (material.textureID) glDeleteTextures(1, &material.textureID);
    }
}

bool Obj3D::load() {
    if (!loadOBJ(objPath)) {
        std::cerr << "Erro ao carregar arquivo OBJ: " << objPath << std::endl;
        return false;
    }
    
    if (!mtlPath.empty() && !loadMTL(mtlPath)) {
        std::cerr << "Erro ao carregar arquivo MTL: " << mtlPath << std::endl;
    }
    
    setupMesh();
    calculateBoundingBox();
    return true;
}

bool Obj3D::loadOBJ(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> tempPositions;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTexCoords;
    
    std::string currentGroupName = "default";
    std::string currentMaterial = "";
    Group currentGroup;
    currentGroup.name = currentGroupName;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "mtllib") {
            iss >> mtlPath;
            // Ajustar caminho do MTL relativo ao OBJ
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                mtlPath = path.substr(0, lastSlash + 1) + mtlPath;
            }
        }
        else if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            tempPositions.push_back(pos);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "vt") {
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            tempTexCoords.push_back(texCoord);
        }
        else if (prefix == "g" || prefix == "o") {
            if (!currentGroup.indices.empty()) {
                groups.push_back(currentGroup);
                currentGroup = Group();
            }
            iss >> currentGroupName;
            currentGroup.name = currentGroupName;
            currentGroup.materialName = currentMaterial;
        }
        else if (prefix == "usemtl") {
            iss >> currentMaterial;
            currentGroup.materialName = currentMaterial;
        }
        else if (prefix == "f") {
            std::vector<unsigned int> faceIndices;
            std::string vertex;
            
            while (iss >> vertex) {
                std::istringstream vertexStream(vertex);
                std::string posIndex, texIndex, normalIndex;
                
                std::getline(vertexStream, posIndex, '/');
                std::getline(vertexStream, texIndex, '/');
                std::getline(vertexStream, normalIndex);
                
                Vertex v;
                
                // Position (obrigatório)
                if (!posIndex.empty()) {
                    int idx = std::stoi(posIndex) - 1;
                    if (idx >= 0 && idx < tempPositions.size()) {
                        v.position = tempPositions[idx];
                    }
                }
                
                // Texture coordinates (opcional)
                if (!texIndex.empty()) {
                    int idx = std::stoi(texIndex) - 1;
                    if (idx >= 0 && idx < tempTexCoords.size()) {
                        v.texCoords = tempTexCoords[idx];
                    }
                }
                
                // Normal (opcional)
                if (!normalIndex.empty()) {
                    int idx = std::stoi(normalIndex) - 1;
                    if (idx >= 0 && idx < tempNormals.size()) {
                        v.normal = tempNormals[idx];
                    }
                } else {
                    v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // Normal padrão
                }
                
                vertices.push_back(v);
                faceIndices.push_back(vertices.size() - 1);
            }
            
            // Triangular faces com 4+ vértices
            std::vector<unsigned int> triangles = triangulatePolygon(faceIndices);
            currentGroup.indices.insert(currentGroup.indices.end(), triangles.begin(), triangles.end());
        }
    }
    
    // Adicionar último grupo
    if (!currentGroup.indices.empty()) {
        groups.push_back(currentGroup);
    }
    
    file.close();
    return true;
}

std::vector<unsigned int> Obj3D::triangulatePolygon(const std::vector<unsigned int>& faceIndices) {
    std::vector<unsigned int> triangles;
    
    if (faceIndices.size() < 3) return triangles;
    
    // Triangulação em leque (fan triangulation)
    for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
        triangles.push_back(faceIndices[0]);
        triangles.push_back(faceIndices[i]);
        triangles.push_back(faceIndices[i + 1]);
    }
    
    return triangles;
}

bool Obj3D::loadMTL(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir o arquivo MTL: " << path << std::endl;
        return false;
    }

    Material currentMaterial;
    bool hasMaterial = false;
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "newmtl") {
            if (hasMaterial) {
                materials.push_back(currentMaterial);
            }
            currentMaterial = Material();
            iss >> currentMaterial.name;
            hasMaterial = true;
        }
        else if (prefix == "Ka") {
            iss >> currentMaterial.ambient.x >> currentMaterial.ambient.y >> currentMaterial.ambient.z;
        }
        else if (prefix == "Kd") {
            iss >> currentMaterial.diffuse.x >> currentMaterial.diffuse.y >> currentMaterial.diffuse.z;
        }
        else if (prefix == "Ks") {
            iss >> currentMaterial.specular.x >> currentMaterial.specular.y >> currentMaterial.specular.z;
        }
        else if (prefix == "Ns") {
            iss >> currentMaterial.shininess;
        }
        else if (prefix == "map_Kd") {
            iss >> currentMaterial.diffuseTexture;
            // Carregar textura
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                currentMaterial.diffuseTexture = path.substr(0, lastSlash + 1) + currentMaterial.diffuseTexture;
            }
            currentMaterial.textureID = loadTexture(currentMaterial.diffuseTexture);
        }
    }
    
    if (hasMaterial) {
        materials.push_back(currentMaterial);
    }
    
    file.close();
    return true;
}

unsigned int Obj3D::loadTexture(const std::string& path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Falha ao carregar textura: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Obj3D::setupMesh() {
    for (auto& group : groups) {
        glGenVertexArrays(1, &group.VAO);
        glGenBuffers(1, &group.VBO);
        glGenBuffers(1, &group.EBO);

        glBindVertexArray(group.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, group.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, group.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, group.indices.size() * sizeof(unsigned int), &group.indices[0], GL_STATIC_DRAW);

        // Posições dos vértices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Normais dos vértices
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // Coordenadas de textura dos vértices
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
}

void Obj3D::calculateBoundingBox() {
    if (vertices.empty()) return;
    
    boundingBoxMin = boundingBoxMax = vertices[0].position;
    
    for (const auto& vertex : vertices) {
        boundingBoxMin.x = std::min(boundingBoxMin.x, vertex.position.x);
        boundingBoxMin.y = std::min(boundingBoxMin.y, vertex.position.y);
        boundingBoxMin.z = std::min(boundingBoxMin.z, vertex.position.z);
        
        boundingBoxMax.x = std::max(boundingBoxMax.x, vertex.position.x);
        boundingBoxMax.y = std::max(boundingBoxMax.y, vertex.position.y);
        boundingBoxMax.z = std::max(boundingBoxMax.z, vertex.position.z);
    }
}

void Obj3D::render(unsigned int shaderProgram) {
    glUseProgram(shaderProgram);
    
    // Enviar matriz de modelo para o shader
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    for (const auto& group : groups) {
        // Encontrar e aplicar material
        Material* material = nullptr;
        for (auto& mat : materials) {
            if (mat.name == group.materialName) {
                material = &mat;
                break;
            }
        }
        
        if (material) {
            // Aplicar propriedades do material
            int ambientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
            int diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
            int specularLoc = glGetUniformLocation(shaderProgram, "material.specular");
            int shininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
            
            glUniform3fv(ambientLoc, 1, glm::value_ptr(material->ambient));
            glUniform3fv(diffuseLoc, 1, glm::value_ptr(material->diffuse));
            glUniform3fv(specularLoc, 1, glm::value_ptr(material->specular));
            glUniform1f(shininessLoc, material->shininess);
            
            // Bind texture
            if (material->textureID) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, material->textureID);
                int textureLoc = glGetUniformLocation(shaderProgram, "diffuseTexture");
                glUniform1i(textureLoc, 0);
            }
        }
        
        glBindVertexArray(group.VAO);
        glDrawElements(GL_TRIANGLES, group.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Obj3D::setPosition(const glm::vec3& position) {
    // Implementar lógica de posição
}

void Obj3D::setRotation(const glm::vec3& rotation) {
    // Implementar lógica de rotação
}

void Obj3D::setScale(const glm::vec3& scale) {
    // Implementar lógica de escala
}

void Obj3D::updateModelMatrix() {
    // Implementar atualização da matriz de modelo
    modelMatrix = glm::mat4(1.0f);
}

bool Obj3D::checkCollision(const glm::vec3& point) const {
    // Implementar detecção de colisão com ponto
    glm::vec4 transformedPoint = glm::inverse(modelMatrix) * glm::vec4(point, 1.0f);
    
    return (transformedPoint.x >= boundingBoxMin.x && transformedPoint.x <= boundingBoxMax.x &&
            transformedPoint.y >= boundingBoxMin.y && transformedPoint.y <= boundingBoxMax.y &&
            transformedPoint.z >= boundingBoxMin.z && transformedPoint.z <= boundingBoxMax.z);
}

bool Obj3D::checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    // Implementar interseção raio-bounding box
    glm::vec4 transformedOrigin = glm::inverse(modelMatrix) * glm::vec4(rayOrigin, 1.0f);
    glm::vec4 transformedDirection = glm::inverse(modelMatrix) * glm::vec4(rayDirection, 0.0f);
    
    glm::vec3 origin = glm::vec3(transformedOrigin);
    glm::vec3 direction = glm::normalize(glm::vec3(transformedDirection));
    
    float tmin = (boundingBoxMin.x - origin.x) / direction.x;
    float tmax = (boundingBoxMax.x - origin.x) / direction.x;
    
    if (tmin > tmax) std::swap(tmin, tmax);
    
    float tymin = (boundingBoxMin.y - origin.y) / direction.y;
    float tymax = (boundingBoxMax.y - origin.y) / direction.y;
    
    if (tymin > tymax) std::swap(tymin, tymax);
    
    if ((tmin > tymax) || (tymin > tmax)) return false;
    
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;
    
    float tzmin = (boundingBoxMin.z - origin.z) / direction.z;
    float tzmax = (boundingBoxMax.z - origin.z) / direction.z;
    
    if (tzmin > tzmax) std::swap(tzmin, tzmax);
    
    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    
    if (tzmin > tmin) tmin = tzmin;
    
    distance = tmin > 0 ? tmin : tmax;
    return distance > 0;
}