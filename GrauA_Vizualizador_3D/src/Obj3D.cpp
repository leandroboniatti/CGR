#include "OBJ3D.h"
#include <iostream>

OBJ3D::OBJ3D() 
    : transform(1.0f), 
      position (0.0f), 
      rotation (0.0f), 
      scale    (1.0f), 
      eliminable(true), 
      name(""),
      textureID(0),
      hasTexture(false)
    { updateTransform(); }

OBJ3D::OBJ3D(string& objName)
    : transform(1.0f),
      position (0.0f),
      rotation (0.0f),
      scale    (1.0f),
      eliminable(true),
      name(objName),
      textureID(0),
      hasTexture(false)
    { updateTransform(); }

OBJ3D::~OBJ3D() { mesh.cleanup(); }

bool OBJ3D::loadObject(string& path) {

    if (!mesh.readObjectModel(path)) {
        cerr << "Falha ao carregar arquivo OBJ: " << path << endl;
        return false;
    }

    cout << "Arquivo OBJ3D \"" << name << "\" carregado com sucesso de: " << path << endl;
    return true;
}

void OBJ3D::render(const Shader& shader) const {
    shader.setMat4("model", transform);
    
    // Set texture uniforms
    if (hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        shader.setBool("hasDiffuseMap", true);
        shader.setInt("diffuseMap", 0);
    } else {
        shader.setBool("hasDiffuseMap", false);
    }
    
    // Set default object color
    shader.setVec3("objectColor", glm::vec3(0.7f, 0.7f, 0.7f));
    
    mesh.render(shader);
}

void OBJ3D::setPosition(const glm::vec3& pos) {
    position = pos;
    updateTransform();
}

void OBJ3D::setRotation(const glm::vec3& rot) {
    rotation = rot;
    updateTransform();
}

void OBJ3D::setScale(const glm::vec3& scl) {
    scale = scl;
    updateTransform();
}

void OBJ3D::setEliminable(bool canEliminate) {
    eliminable = canEliminate;
}

void OBJ3D::setTexture(const string& texturePath) {
    if (!texturePath.empty()) {
        textureID = Texture::loadTexture(texturePath);
        hasTexture = (textureID != 0);
        if (hasTexture) {
            cout << "Textura carregada para objeto \"" << name << "\": " << texturePath << endl;
        } else {
            cerr << "Falha ao carregar textura para objeto \"" << name << "\": " << texturePath << endl;
        }
    } else {
        hasTexture = false;
        textureID = 0;
    }
}

void OBJ3D::translate(const glm::vec3& offset) {
    position += offset;
    updateTransform();
}

void OBJ3D::rotate(const glm::vec3& angles) {
    rotation += angles;
    updateTransform();
}

void OBJ3D::scaleBy(const glm::vec3& factor) {
    scale *= factor;
    updateTransform();
}


// Atualiza a matriz de transformação (model matrix) com base na posição, rotação e escala
void OBJ3D::updateTransform() {

    transform = glm::mat4(1.0f);

    // Aplica transformações na ordem: Escala -> Rotação -> Translação

    // translação
    transform = glm::translate(transform, position);
    
    // Aplica rotações
    transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Aplica escala
    transform = glm::scale(transform, scale);
}


// Retorna a bounding box do objeto 3D transformada pela matriz de transformação
BoundingBox OBJ3D::getTransformedBoundingBox() const {
    BoundingBox transformedBB;

    // Transforma todos os 8 cantos da caixa delimitadora
    glm::vec3 corners[8] = {
        mesh.boundingBox.min,
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.min.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.max.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.min.y, mesh.boundingBox.max.z),
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.max.y, mesh.boundingBox.min.z),
        glm::vec3(mesh.boundingBox.max.x, mesh.boundingBox.min.y, mesh.boundingBox.max.z),
        glm::vec3(mesh.boundingBox.min.x, mesh.boundingBox.max.y, mesh.boundingBox.max.z),
        mesh.boundingBox.max
    };
    
    for (int i = 0; i < 8; i++) {
        glm::vec4 transformedCorner = transform * glm::vec4(corners[i], 1.0f);
        transformedBB.expand(glm::vec3(transformedCorner));
    }
    
    return transformedBB;
}


// Testa interseção do raio com a bounding box (retorna true se houver interseção)
// Se houver interseção, retorna a distância até o ponto de interseção mais próximo
// objetos muito rápidos podem atravessar objetos sem detectar colisão !!!
bool OBJ3D::rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const {
    // Transforma as informações do "raio" para o espaço do objeto ("Local Space")
    glm::mat4 invTransform = glm::inverse(transform);   // gera a matriz inversa da transformação
    glm::vec4 localOrigin = invTransform * glm::vec4(rayOrigin, 1.0f); // ponto de origem do raio no espaço do objeto
    glm::vec4 localDirection = invTransform * glm::vec4(rayDirection, 0.0f); // direção do raio no espaço do objeto
    
    // verifica interseção com a bounding box da malha no espaço do objeto
    return mesh.rayIntersect(glm::vec3(localOrigin), glm::normalize(glm::vec3(localDirection)), distance);
}