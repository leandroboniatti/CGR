#pragma once

#include <vector>
#include <glm/glm.hpp>

class Face {
private:
    std::vector<int> vertexIndices;
    std::vector<int> textureIndices;
    std::vector<int> normalIndices;

public:
    Face();
    ~Face();

    void addVertexIndex(int index);
    void addTextureIndex(int index);
    void addNormalIndex(int index);

    const std::vector<int>& getVertexIndices() const;
    const std::vector<int>& getTextureIndices() const;
    const std::vector<int>& getNormalIndices() const;

    // Converte faces de 4+ lados em triângulos
    std::vector<Face> triangulate() const;
    bool isTriangle() const;
    size_t getVertexCount() const;
};