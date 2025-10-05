#pragma once
#include <vector>
#include <string>
#include <map>
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
    glm::vec3 emission;
    float shininess;
    float transparency;
    float opticalDensity;
    int illuminationModel;
    
    // Texturas
    std::string diffuseTexture;
    std::string normalTexture;
    std::string specularTexture;
    unsigned int diffuseTextureID;
    unsigned int normalTextureID;
    unsigned int specularTextureID;
    
    Material() : 
        name(""),
        ambient(0.2f, 0.2f, 0.2f), 
        diffuse(0.8f, 0.8f, 0.8f), 
        specular(1.0f, 1.0f, 1.0f),
        emission(0.0f, 0.0f, 0.0f),
        shininess(32.0f),
        transparency(1.0f),
        opticalDensity(1.0f),
        illuminationModel(2),
        diffuseTextureID(0),
        normalTextureID(0),
        specularTextureID(0) {}
        
    // Material padrão
    static Material getDefault() {
        Material defaultMat;
        defaultMat.name = "default";
        defaultMat.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
        defaultMat.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        defaultMat.specular = glm::vec3(0.5f, 0.5f, 0.5f);
        defaultMat.shininess = 16.0f;
        return defaultMat;
    }
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
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
    bool eliminable;
    std::string objPath;
    std::string mtlPath;
    
    // Cache de texturas para evitar carregar a mesma textura múltiplas vezes
    static std::map<std::string, unsigned int> textureCache;

    bool loadOBJ(const std::string& path);
    bool loadMTL(const std::string& path);
    void setupMesh();
    void calculateBoundingBox();
    unsigned int loadTexture(const std::string& path);
    Material* findMaterial(const std::string& materialName);
    void applyMaterial(const Material& material, unsigned int shaderProgram);
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
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }
    glm::vec3 getBoundingBoxMin() const { return boundingBoxMin; }
    glm::vec3 getBoundingBoxMax() const { return boundingBoxMax; }
    bool isEliminable() const { return eliminable; }
    void setEliminable(bool value) { eliminable = value; }
    
    // Métodos de materiais
    const std::vector<Material>& getMaterials() const { return materials; }
    void addMaterial(const Material& material);
    bool updateMaterial(const std::string& name, const Material& newMaterial);
    void setDefaultMaterial();
    void clearTextureCache();
    
    // Métodos utilitários para materiais
    static Material createMetallicMaterial(const glm::vec3& color, float roughness = 0.1f);
    static Material createPlasticMaterial(const glm::vec3& color, float shininess = 64.0f);
    static Material createEmissiveMaterial(const glm::vec3& color, float intensity = 1.0f);
};