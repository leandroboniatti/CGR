#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "Shader.h"

class Renderer {
public:
    void draw(const std::vector<Mesh>& meshes, const Shader& shader) const;
};
