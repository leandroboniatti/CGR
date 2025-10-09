#ifndef MESH_H
#define MESH_H

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Group.h"
#include "Material.h"

using namespace std;

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    
    BoundingBox() : min(FLT_MAX), max(-FLT_MAX) {}
    
    void expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
    
    glm::vec3 center() const {
        return (min + max) * 0.5f;
    }
    
    glm::vec3 size() const {
        return max - min;
    }
    
    float radius() const {
        return glm::length(size()) * 0.5f;
    }
};

class Mesh {
public:
    vector<glm::vec3> vertices;  // Vetor com os vértices do objeto 3D
    vector<glm::vec2> texCoords; // Vetor com as coordenadas de textura do objeto 3D
    vector<glm::vec3> normals;   // Vetor com as normais do objeto 3D
    vector<Group> groups;        // Grupos que compõem a malha
    map<string, Material> materials;  // Mapa de materiais que podem ser usados pelos objetos 3D
    
    BoundingBox boundingBox;
    
    Mesh();
    ~Mesh();
    
    bool readOBJ(string& path);
    //void setupBuffers();
    void render(const class Shader& shader) const;
    void cleanup();
    
    void calculateBoundingBox();
    bool rayIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, 
                     float& distance) const;
    
private:
    void calculateNormals();
};

#endif