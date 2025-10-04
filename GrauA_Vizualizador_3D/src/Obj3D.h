#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    std::string diffuseTexture;
    unsigned int textureID;
    
    Material() : ambient(0.2f), diffuse(0.8f), specular(1.0f), shininess(32.0f), textureID(0) {}
};

struct Group {
    std::string name;
    std::string materialName;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    
    Group() : VAO(0), VBO(0), EBO(0) {}
};

class Obj3D {
private:
    std::vector<Vertex> vertices;
    std::vector<Group> groups;
    std::vector<Material> materials;
    glm::mat4 modelMatrix;
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
    bool eliminable;
    std::string objPath;
    std::string mtlPath;

    bool loadOBJ(const std::string& path);
    bool loadMTL(const std::string& path);
    void setupMesh();
    void calculateBoundingBox();
    unsigned int loadTexture(const std::string& path);
    std::vector<unsigned int> triangulatePolygon(const std::vector<unsigned int>& faceIndices);

public:
    Obj3D(const std::string& objFile, const glm::vec3& position = glm::vec3(0.0f), 
          const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f),
          bool eliminable = true);
    
    ~Obj3D();
    
    bool load();
    void render(unsigned int shaderProgram);
    
    // Transformações
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);
    void updateModelMatrix();
    
    // Colisão
    bool checkCollision(const glm::vec3& point) const;
    bool checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const;
    
    // Getters
    glm::mat4 getModelMatrix() const { return modelMatrix; }
    glm::vec3 getBoundingBoxMin() const { return boundingBoxMin; }
    glm::vec3 getBoundingBoxMax() const { return boundingBoxMax; }
    bool isEliminable() const { return eliminable; }
    void setEliminable(bool value) { eliminable = value; }
};