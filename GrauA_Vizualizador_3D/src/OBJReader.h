#ifndef OBJREADER_H
#define OBJREADER_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Material.h"
#include "Group.h"

class OBJReader {
public:
    static bool loadOBJ(const std::string& path,
                        std::vector<glm::vec3>& vertices,
                        std::vector<glm::vec2>& texCoords,
                        std::vector<glm::vec3>& normals,
                        std::vector<Group>& groups,
                        std::map<std::string, Material>& materials);
    
    static bool loadMTL(const std::string& path, std::map<std::string, Material>& materials);
    
private:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
    static std::string getDirectory(const std::string& filepath);
    
    static void parseFace(const std::string& faceStr, Face& face);
    static void parseVertice(const std::string& line, std::vector<glm::vec3>& vertices);
    static void parseTexCoord(const std::string& line, std::vector<glm::vec2>& texCoords);
    static void parseNormal(const std::string& line, std::vector<glm::vec3>& normals);
};

#endif