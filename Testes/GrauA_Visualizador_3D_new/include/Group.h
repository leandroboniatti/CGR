#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Face.h"
#include "Material.h"

class Group {
private:
    std::string name;
    std::vector<Face> faces;
    std::shared_ptr<Material> material;
    
    // OpenGL objects
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    std::vector<float> vertexData;
    std::vector<unsigned int> indices;
    bool isSetup;

public:
    Group();
    Group(const std::string& groupName);
    ~Group();

    // Getters
    const std::string& getName() const;
    const std::vector<Face>& getFaces() const;
    std::shared_ptr<Material> getMaterial() const;
    unsigned int getVAO() const;
    size_t getIndexCount() const;

    // Setters
    void setName(const std::string& groupName);
    void setMaterial(std::shared_ptr<Material> mat);

    // Face management
    void addFace(const Face& face);
    void clearFaces();

    // OpenGL setup
    void setupVAO(const std::vector<glm::vec3>& vertices,
                  const std::vector<glm::vec2>& texCoords,
                  const std::vector<glm::vec3>& normals);
    void render();
    void cleanup();

private:
    void buildVertexData(const std::vector<glm::vec3>& vertices,
                        const std::vector<glm::vec2>& texCoords,
                        const std::vector<glm::vec3>& normals);
};