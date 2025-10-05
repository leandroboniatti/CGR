#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Face {
public:
    std::vector<int> verts;
    std::vector<int> norms;
    std::vector<int> texts;
    
    // Convert n-gon face to triangles
    std::vector<Face> triangulate() const;
};