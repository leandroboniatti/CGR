#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <glm/glm.hpp>

using namespace std;

class Material {
public:
    string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    string diffuseTexture;
    string normalTexture;
    string specularTexture;

    unsigned int diffuseTextureID;
    unsigned int normalTextureID;
    unsigned int specularTextureID;
    
    Material(); // Construtor padrão
    Material(const string& materialName);

    void loadTextures();
    void bind() const;
    void unbind() const;
};

#endif