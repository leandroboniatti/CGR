#include "Material.h"
#include "Texture.h"
#include <glad/glad.h>
#include <iostream>

Material::Material() 
    : name(""), 
      ambient(0.2f, 0.2f, 0.2f), 
      diffuse(0.8f, 0.8f, 0.8f), 
      specular(1.0f, 1.0f, 1.0f),
      shininess(32.0f),
      diffuseTextureID(0),
      normalTextureID(0),
      specularTextureID(0) {}

Material::Material(const string& materialName)
    : name(materialName),
      ambient(0.2f, 0.2f, 0.2f),
      diffuse(0.8f, 0.8f, 0.8f),
      specular(1.0f, 1.0f, 1.0f),
      shininess(32.0f),
      diffuseTextureID(0),
      normalTextureID(0),
      specularTextureID(0) {}

void Material::loadTextures() {
    if (!diffuseTexture.empty()) {
        diffuseTextureID = Texture::loadTexture(diffuseTexture);
    }
    if (!normalTexture.empty()) {
        normalTextureID = Texture::loadTexture(normalTexture);
    }
    if (!specularTexture.empty()) {
        specularTextureID = Texture::loadTexture(specularTexture);
    }
}

void Material::bind() const {
    if (diffuseTextureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTextureID);
    }
    if (normalTextureID != 0) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTextureID);
    }
    if (specularTextureID != 0) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularTextureID);
    }
}

void Material::unbind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
}