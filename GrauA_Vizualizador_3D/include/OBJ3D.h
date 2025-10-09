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
    glm::mat4 transform;
    
    glm::vec3 position;
    glm::vec3 rotation; // Euler angles in radians
    glm::vec3 scale;
    
    bool eliminable;
    string name, modelPath, texturePath;
    
    // Texture support
    unsigned int textureID;
    bool hasTexture;
    
    OBJ3D();
    OBJ3D(string& objName);

    ~OBJ3D();
    
    bool readFile(string& path);
    void render(const Shader& shader) const;
    
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
    bool continuousRayIntersect(const glm::vec3& rayStart, const glm::vec3& rayEnd,
                               float& distance, glm::vec3& hitPoint, glm::vec3& hitNormal) const;

private:
    void updateTransform();
};

#endif