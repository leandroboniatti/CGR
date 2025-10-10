#ifndef OBJ3D_H
#define OBJ3D_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

using namespace std;

class OBJ3D {
public:
    Mesh mesh;  // malha do objeto 3D
    glm::mat4 transform;    // matriz de transformação do objeto (model matrix)
    glm::vec3 position;     // posição do objeto
    glm::vec3 rotation;     // ângulos de rotação do objeto (em radianos)
    glm::vec3 scale;        // escala do objeto

    bool eliminable;
    string name, modelPath, texturePath;
    
    // Texture support
    unsigned int textureID;
    bool hasTexture;
    
    OBJ3D();

    OBJ3D(string& objName);

    ~OBJ3D();

    // Carrega um objeto 3D a partir de um arquivo
    bool loadObject(string& path);

    // Renderiza o objeto 3D usando o shader fornecido
    void render(const Shader& shader) const;
    
    // Define a posição, rotação e escala do objeto e atualiza a matriz de transformação
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale   (const glm::vec3& scl);
    void setEliminable(bool canEliminate);
    void setTexture(const string& texturePath);
    
    void translate(const glm::vec3& offset);
    void rotate(const glm::vec3& angles);
    void scaleBy(const glm::vec3& factor);

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getRotation() const { return rotation; }
    glm::vec3 getScale() const { return scale; }
    bool isEliminable() const { return eliminable; }

    BoundingBox getTransformedBoundingBox() const;

    bool rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const;
    
    // Testa a interseção de um segmento de reta com a malha do objeto. O segmento é
    // definido por dois pontos: início (segStart) e fim (segEnd). "segStart" é o ponto atual
    // do projétil e "segEnd" é o ponto futuro do projétil. Se houver interseção, retorna o
    // ponto de interseção mais próximo (hitPoint), a distância dele ao ponto de origem do
    // segmento (segStart) e a normal da face atingida (hitNormal), para eventual reflexão.
    bool continuousRayIntersect(const glm::vec3& rayStart, const glm::vec3& rayEnd,
                               float& distance, glm::vec3& hitPoint, glm::vec3& hitNormal) const;

//private:
    void updateTransform();
};

#endif