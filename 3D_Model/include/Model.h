#pragma once
#include <string>
#include <vector>
#include "Mesh.h"

class Model {
public:
    bool loadOBJ(const std::string& path, bool verbose = true);
    const std::vector<Mesh>& meshes() const { return m_meshes; }
private:
    std::vector<Mesh> m_meshes;
    std::string m_directory;
};
