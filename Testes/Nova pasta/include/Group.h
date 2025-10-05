#pragma once

#include <vector>
#include <string>
#include "Face.h"
#include "Material.h"

class Group {
public:
    Group();
    Group(const std::string& name);
    ~Group();
    
    // Propriedades
    std::string name;
    std::vector<Face> faces;
    Material* material;
    
    // OpenGL objects
    unsigned int VAO;
    unsigned int VBO;
    unsigned int numVertices;
    
    // Métodos
    void addFace(const Face& face);
    void setMaterial(Material* mat);
    void setupBuffers(const std::vector<glm::vec3>& vertices,
                     const std::vector<glm::vec2>& texCoords,
                     const std::vector<glm::vec3>& normals);
    void render();
    void cleanup();
    
private:
    void generateBuffers();
};