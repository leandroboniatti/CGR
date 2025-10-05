#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <glm/glm.hpp>

class Material {
public:
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    std::string diffuseTexture;
    std::string normalTexture;
    std::string specularTexture;
    
    unsigned int diffuseTextureID;
    unsigned int normalTextureID;
    unsigned int specularTextureID;
    
    Material();
    Material(const std::string& materialName);
    
    void loadTextures();
    void bind() const;
    void unbind() const;
};

#endif