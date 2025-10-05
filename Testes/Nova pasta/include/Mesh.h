#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Group.h"

class Mesh {
public:
    Mesh();
    ~Mesh();
    
    // Dados do mesh
    std::string name;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<Group> groups;
    
    // Métodos
    void addVertex(const glm::vec3& vertex);
    void addTexCoord(const glm::vec2& texCoord);
    void addNormal(const glm::vec3& normal);
    void addGroup(const Group& group);
    void setupBuffers();
    void render();
    void cleanup();
    
    // Bounding box
    glm::vec3 minBounds;
    glm::vec3 maxBounds;
    void calculateBounds();
    bool intersectsBoundingBox(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const;
};