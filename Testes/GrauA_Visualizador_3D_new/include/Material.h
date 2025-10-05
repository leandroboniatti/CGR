#pragma once

#include <string>
#include <glm/glm.hpp>

class Material {
private:
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    std::string diffuseTexturePath;
    std::string normalTexturePath;
    unsigned int diffuseTextureID;
    unsigned int normalTextureID;

public:
    Material();
    Material(const std::string& materialName);
    ~Material();

    // Getters
    const std::string& getName() const;
    const glm::vec3& getAmbient() const;
    const glm::vec3& getDiffuse() const;
    const glm::vec3& getSpecular() const;
    float getShininess() const;
    const std::string& getDiffuseTexturePath() const;
    const std::string& getNormalTexturePath() const;
    unsigned int getDiffuseTextureID() const;
    unsigned int getNormalTextureID() const;

    // Setters
    void setName(const std::string& materialName);
    void setAmbient(const glm::vec3& amb);
    void setDiffuse(const glm::vec3& diff);
    void setSpecular(const glm::vec3& spec);
    void setShininess(float shine);
    void setDiffuseTexturePath(const std::string& path);
    void setNormalTexturePath(const std::string& path);
    void setDiffuseTextureID(unsigned int id);
    void setNormalTextureID(unsigned int id);

    // Métodos de utilidade
    bool hasTexture() const;
    bool hasNormalMap() const;
};