#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Mesh.h"
#include "Material.h"

class OBJReader {
public:
    static bool loadOBJ(const std::string& filepath, Mesh& mesh, 
                        std::unordered_map<std::string, Material>& materials);
    static bool loadMTL(const std::string& filepath, 
                        std::unordered_map<std::string, Material>& materials);
private:
    static std::string getDirectory(const std::string& filepath);
};