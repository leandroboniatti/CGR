#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Obj3D.h"

class OBJReader {
public:
    static Obj3D* load(const std::string& filename);
    
private:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static glm::vec3 parseVec3(const std::string& line);
    static glm::vec2 parseVec2(const std::string& line);
    static std::vector<int> parseFace(const std::string& line);
    static std::vector<std::vector<int>> triangulateFace(const std::vector<int>& face);
};