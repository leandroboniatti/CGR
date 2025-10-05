#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <string>
#include "Face.h"
#include "Material.h"

class Group {
public:
    std::string name;
    std::string materialName;
    std::vector<Face> faces;
    Material material;
    
    // OpenGL objects
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    std::vector<float> vertices;  // Vértices expandidos para renderização
    std::vector<unsigned int> indices;
    
    Group();
    Group(const std::string& groupName);
    ~Group();
    
    void addFace(const Face& face);
    void setupBuffers(const std::vector<glm::vec3>& objVertices,
                     const std::vector<glm::vec2>& objTexCoords,
                     const std::vector<glm::vec3>& objNormals);
    void render() const;
    void cleanup();
    
private:
    void generateVertexData(const std::vector<glm::vec3>& objVertices,
                           const std::vector<glm::vec2>& objTexCoords,
                           const std::vector<glm::vec3>& objNormals);
};

#endif