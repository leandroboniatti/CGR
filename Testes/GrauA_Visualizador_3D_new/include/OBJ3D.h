#pragma once

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Group.h"

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
    float radius;

    BoundingBox() : min(0.0f), max(0.0f), center(0.0f), radius(0.0f) {}
    
    void calculate(const std::vector<glm::vec3>& vertices);
    bool intersects(const BoundingBox& other) const;
    bool intersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const;
};

class OBJ3D {
private:
    std::string filename;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<glm::vec3> normals;
    std::vector<std::shared_ptr<Group>> groups;
    
    // Transformação
    glm::mat4 transformMatrix;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    
    // Bounding box para colisão
    BoundingBox boundingBox;
    
    // Propriedades do objeto
    bool eliminable;
    bool visible;

public:
    OBJ3D();
    OBJ3D(const std::string& objFilename);
    ~OBJ3D();

    // Getters
    const std::string& getFilename() const;
    const std::vector<glm::vec3>& getVertices() const;
    const std::vector<glm::vec2>& getTextureCoords() const;
    const std::vector<glm::vec3>& getNormals() const;
    const std::vector<std::shared_ptr<Group>>& getGroups() const;
    const glm::mat4& getTransformMatrix() const;
    const glm::vec3& getPosition() const;
    const glm::vec3& getRotation() const;
    const glm::vec3& getScale() const;
    const BoundingBox& getBoundingBox() const;
    bool isEliminable() const;
    bool isVisible() const;

    // Setters
    void setFilename(const std::string& objFilename);
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(const glm::vec3& scl);
    void setEliminable(bool elim);
    void setVisible(bool vis);

    // Vertex data management
    void addVertex(const glm::vec3& vertex);
    void addTextureCoord(const glm::vec2& texCoord);
    void addNormal(const glm::vec3& normal);
    void addGroup(std::shared_ptr<Group> group);

    // Transform operations
    void translate(const glm::vec3& offset);
    void rotate(const glm::vec3& angles);
    void scaleObject(const glm::vec3& factors);
    void updateTransformMatrix();

    // Rendering
    void setupVAOs();
    void render();
    void cleanup();

    // Collision
    void calculateBoundingBox();
    bool checkCollision(const OBJ3D& other) const;
    bool checkRayIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float& distance) const;
};