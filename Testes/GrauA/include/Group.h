#pragma once
#include <string>
#include <vector>
#include "Face.h"

class Group {
public:
    std::string name;
    std::string material;
    std::vector<Face> faces;
    
    // OpenGL objects
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int vertexCount = 0;
    
    void setupGL(const std::vector<glm::vec3>& vertices,
                 const std::vector<glm::vec3>& normals, 
                 const std::vector<glm::vec2>& texCoords);
};