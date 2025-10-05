#include "Material.h"

Material::Material() 
    : name("default")
    , ambient(0.2f, 0.2f, 0.2f)
    , diffuse(0.8f, 0.8f, 0.8f)
    , specular(1.0f, 1.0f, 1.0f)
    , shininess(32.0f)
    , diffuseTextureID(0)
    , normalTextureID(0) {
}

Material::Material(const std::string& materialName)
    : name(materialName)
    , ambient(0.2f, 0.2f, 0.2f)
    , diffuse(0.8f, 0.8f, 0.8f)
    , specular(1.0f, 1.0f, 1.0f)
    , shininess(32.0f)
    , diffuseTextureID(0)
    , normalTextureID(0) {
}

Material::~Material() {
}

const std::string& Material::getName() const {
    return name;
}

const glm::vec3& Material::getAmbient() const {
    return ambient;
}

const glm::vec3& Material::getDiffuse() const {
    return diffuse;
}

const glm::vec3& Material::getSpecular() const {
    return specular;
}

float Material::getShininess() const {
    return shininess;
}

const std::string& Material::getDiffuseTexturePath() const {
    return diffuseTexturePath;
}

const std::string& Material::getNormalTexturePath() const {
    return normalTexturePath;
}

unsigned int Material::getDiffuseTextureID() const {
    return diffuseTextureID;
}

unsigned int Material::getNormalTextureID() const {
    return normalTextureID;
}

void Material::setName(const std::string& materialName) {
    name = materialName;
}

void Material::setAmbient(const glm::vec3& amb) {
    ambient = amb;
}

void Material::setDiffuse(const glm::vec3& diff) {
    diffuse = diff;
}

void Material::setSpecular(const glm::vec3& spec) {
    specular = spec;
}

void Material::setShininess(float shine) {
    shininess = shine;
}

void Material::setDiffuseTexturePath(const std::string& path) {
    diffuseTexturePath = path;
}

void Material::setNormalTexturePath(const std::string& path) {
    normalTexturePath = path;
}

void Material::setDiffuseTextureID(unsigned int id) {
    diffuseTextureID = id;
}

void Material::setNormalTextureID(unsigned int id) {
    normalTextureID = id;
}

bool Material::hasTexture() const {
    return diffuseTextureID != 0 || !diffuseTexturePath.empty();
}

bool Material::hasNormalMap() const {
    return normalTextureID != 0 || !normalTexturePath.empty();
}