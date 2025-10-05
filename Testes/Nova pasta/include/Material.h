#pragma once

#include <string>
#include <glm/glm.hpp>

class Material {
public:
    Material();
    Material(const std::string& name);
    ~Material();

    // Propriedades do material
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    
    // Texturas
    unsigned int diffuseTexture;
    std::string texturePath;
    
    // Métodos
    void loadTexture(const std::string& texturePath);
    void bind();
    void unbind();
    
private:
    unsigned int loadTextureFromFile(const std::string& path);
};