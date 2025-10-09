#ifndef GROUP_H
#define GROUP_H

#include <vector>
#include <string>
#include "Face.h"
#include "Material.h"

using namespace std;

class Group {
public:
    string name;
    string materialName;
    vector<Face> faces;
    Material material;
    
    // OpenGL objects
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    vector<float> vertices;  // vetor para guardar os vértices a serem enviados para renderização
    vector<unsigned int> indices;  // Índices para glDrawElements
    
    Group();
    Group(const string& groupName);
    ~Group();

    void addFace(const Face& face);
    void setupBuffers(const vector<glm::vec3>& objVertices,
                      const vector<glm::vec2>& objTexCoords,
                      const vector<glm::vec3>& objNormals   );
    void render() const;
    void cleanup();
    
private:
    void generateVertexData(const vector<glm::vec3>& objVertices,
                            const vector<glm::vec2>& objTexCoords,
                            const vector<glm::vec3>& objNormals   );
};

#endif