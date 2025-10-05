#include "Material.h"
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>

Material::Material() : name("default"), ambient(0.2f), diffuse(0.8f), specular(1.0f), 
                      shininess(32.0f), diffuseTexture(0) {
}

Material::Material(const std::string& name) : name(name), ambient(0.2f), diffuse(0.8f), 
                                             specular(1.0f), shininess(32.0f), diffuseTexture(0) {
}

Material::~Material() {
    if (diffuseTexture != 0) {
        glDeleteTextures(1, &diffuseTexture);
    }
}

void Material::loadTexture(const std::string& path) {
    texturePath = path;
    diffuseTexture = loadTextureFromFile(path);
}

void Material::bind() {
    if (diffuseTexture != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
    }
}

void Material::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Material::loadTextureFromFile(const std::string& path) {
    Texture texture;
    if (texture.loadFromFile(path)) {
        return texture.ID;
    }
    return 0;
}