#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Group.h"

class Mesh {
public:
    std::string mtllib;
    glm::vec3 min, max;  // bounding box
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<Group> groups;
    
    void calculateBounds();
};