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

// Inicializar cache de texturas estático
std::map<std::string, unsigned int> Obj3D::textureCache;

Obj3D::Obj3D(const std::string& objFile, const glm::vec3& position, 
             const glm::vec3& rotation, const glm::vec3& scale, bool eliminable)
    : objPath(objFile), modelMatrix(1.0f), position(position), rotation(rotation), 
      scale(scale), eliminable(eliminable) {

    updateModelMatrix();
}

Obj3D::~Obj3D() {
    for (auto& group : groups) {
        if (group.VAO) glDeleteVertexArrays(1, &group.VAO);
        if (group.VBO) glDeleteBuffers(1, &group.VBO);
        if (group.EBO) glDeleteBuffers(1, &group.EBO);
    }
    
    // Limpeza de texturas (cuidado com o cache compartilhado)
    for (auto& material : materials) {
        if (material.diffuseTextureID) {
            // Verificar se a textura não está sendo usada por outros objetos
            // antes de deletar (implementação simplificada)
        }
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
        else if (prefix == "d" || prefix == "Tr") {
            iss >> currentMaterial.transparency;
        }
        else if (prefix == "Ni") {
            iss >> currentMaterial.opticalDensity;
        }
        else if (prefix == "illum") {
            iss >> currentMaterial.illuminationModel;
        }
        else if (prefix == "Ke") {
            iss >> currentMaterial.emission.x >> currentMaterial.emission.y >> currentMaterial.emission.z;
        }
        else if (prefix == "map_Kd") {
            iss >> currentMaterial.diffuseTexture;
            // Carregar textura
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                currentMaterial.diffuseTexture = path.substr(0, lastSlash + 1) + currentMaterial.diffuseTexture;
            }
            currentMaterial.diffuseTextureID = loadTexture(currentMaterial.diffuseTexture);
        }
        else if (prefix == "map_Ks") {
            iss >> currentMaterial.specularTexture;
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                currentMaterial.specularTexture = path.substr(0, lastSlash + 1) + currentMaterial.specularTexture;
            }
            currentMaterial.specularTextureID = loadTexture(currentMaterial.specularTexture);
        }
        else if (prefix == "map_Bump" || prefix == "bump") {
            iss >> currentMaterial.normalTexture;
            size_t lastSlash = path.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                currentMaterial.normalTexture = path.substr(0, lastSlash + 1) + currentMaterial.normalTexture;
            }
            currentMaterial.normalTextureID = loadTexture(currentMaterial.normalTexture);
        }
    }
    
    if (hasMaterial) {
        materials.push_back(currentMaterial);
    }
    
    file.close();
    return true;
}

unsigned int Obj3D::loadTexture(const std::string& path) {
    // Verificar se a textura já foi carregada
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        return it->second;
    }
    
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB; // Inicializar com valor padrão
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
        
        // Adicionar ao cache
        textureCache[path] = textureID;
        
        std::cout << "Textura carregada com sucesso: " << path << std::endl;
    } else {
        std::cerr << "Falha ao carregar textura: " << path << std::endl;
        stbi_image_free(data);
        
        // Retornar textura branca padrão em caso de erro
        textureID = 0; // Será criada uma textura padrão se necessário
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
        Material* material = findMaterial(group.materialName);
        
        if (!material) {
            // Usar material padrão se nenhum material for encontrado
            Material defaultMat = Material::getDefault();
            applyMaterial(defaultMat, shaderProgram);
        } else {
            applyMaterial(*material, shaderProgram);
        }
        
        glBindVertexArray(group.VAO);
        glDrawElements(GL_TRIANGLES, group.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
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

// Implementações dos novos métodos de material

void Obj3D::setPosition(const glm::vec3& pos) {
    position = pos;
    updateModelMatrix();
}

void Obj3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateModelMatrix();
}

void Obj3D::setScale(const glm::vec3& scl) {
    scale = scl;
    updateModelMatrix();
}

void Obj3D::updateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    
    // Aplicar transformações na ordem: Translação * Rotação * Escala
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
}

Material* Obj3D::findMaterial(const std::string& materialName) {
    for (auto& mat : materials) {
        if (mat.name == materialName) {
            return &mat;
        }
    }
    return nullptr;
}

void Obj3D::applyMaterial(const Material& material, unsigned int shaderProgram) {
    // Aplicar propriedades do material
    int ambientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
    int diffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
    int specularLoc = glGetUniformLocation(shaderProgram, "material.specular");
    int shininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
    int emissionLoc = glGetUniformLocation(shaderProgram, "material.emission");
    
    glUniform3fv(ambientLoc, 1, glm::value_ptr(material.ambient));
    glUniform3fv(diffuseLoc, 1, glm::value_ptr(material.diffuse));
    glUniform3fv(specularLoc, 1, glm::value_ptr(material.specular));
    glUniform1f(shininessLoc, material.shininess);
    
    if (emissionLoc != -1) {
        glUniform3fv(emissionLoc, 1, glm::value_ptr(material.emission));
    }
    
    // Configurar flags de textura
    int hasTextureLoc = glGetUniformLocation(shaderProgram, "hasTexture");
    int hasSpecularTextureLoc = glGetUniformLocation(shaderProgram, "hasSpecularTexture");
    int hasNormalTextureLoc = glGetUniformLocation(shaderProgram, "hasNormalTexture");
    
    // Bind texturas
    if (material.diffuseTextureID && hasTextureLoc != -1) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTextureID);
        int textureLoc = glGetUniformLocation(shaderProgram, "diffuseTexture");
        glUniform1i(textureLoc, 0);
        glUniform1i(hasTextureLoc, 1);
    } else if (hasTextureLoc != -1) {
        glUniform1i(hasTextureLoc, 0);
    }
    
    if (material.specularTextureID && hasSpecularTextureLoc != -1) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, material.specularTextureID);
        int specTextureLoc = glGetUniformLocation(shaderProgram, "specularTexture");
        if (specTextureLoc != -1) {
            glUniform1i(specTextureLoc, 1);
        }
        glUniform1i(hasSpecularTextureLoc, 1);
    } else if (hasSpecularTextureLoc != -1) {
        glUniform1i(hasSpecularTextureLoc, 0);
    }
    
    if (material.normalTextureID && hasNormalTextureLoc != -1) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.normalTextureID);
        int normalTextureLoc = glGetUniformLocation(shaderProgram, "normalTexture");
        if (normalTextureLoc != -1) {
            glUniform1i(normalTextureLoc, 2);
        }
        glUniform1i(hasNormalTextureLoc, 1);
    } else if (hasNormalTextureLoc != -1) {
        glUniform1i(hasNormalTextureLoc, 0);
    }
}

void Obj3D::addMaterial(const Material& material) {
    materials.push_back(material);
}

bool Obj3D::updateMaterial(const std::string& name, const Material& newMaterial) {
    for (auto& mat : materials) {
        if (mat.name == name) {
            mat = newMaterial;
            mat.name = name; // Preservar o nome
            return true;
        }
    }
    return false;
}

void Obj3D::setDefaultMaterial() {
    materials.clear();
    materials.push_back(Material::getDefault());
}

void Obj3D::clearTextureCache() {
    for (auto& pair : textureCache) {
        glDeleteTextures(1, &pair.second);
    }
    textureCache.clear();
}

// Métodos utilitários para criar materiais procedurais

Material Obj3D::createMetallicMaterial(const glm::vec3& color, float roughness) {
    Material material;
    material.name = "metallic";
    material.ambient = color * 0.1f;
    material.diffuse = color * 0.3f;
    material.specular = glm::vec3(0.9f, 0.9f, 0.9f);
    material.shininess = 1.0f / (roughness * roughness) * 128.0f;
    material.emission = glm::vec3(0.0f);
    return material;
}

Material Obj3D::createPlasticMaterial(const glm::vec3& color, float shininess) {
    Material material;
    material.name = "plastic";
    material.ambient = color * 0.2f;
    material.diffuse = color * 0.8f;
    material.specular = glm::vec3(0.6f, 0.6f, 0.6f);
    material.shininess = shininess;
    material.emission = glm::vec3(0.0f);
    return material;
}

Material Obj3D::createEmissiveMaterial(const glm::vec3& color, float intensity) {
    Material material;
    material.name = "emissive";
    material.ambient = glm::vec3(0.0f);
    material.diffuse = color * 0.5f;
    material.specular = glm::vec3(0.0f);
    material.shininess = 1.0f;
    material.emission = color * intensity;
    return material;
}