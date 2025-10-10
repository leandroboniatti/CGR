#ifndef OBJREADER_H
#define OBJREADER_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "Group.h"

using namespace std;

class OBJReader {
public:
    static bool readFileOBJ(const string& path,
                        vector<glm::vec3>& vertices,
                        vector<glm::vec2>& texCoords,
                        vector<glm::vec3>& normals,
                        vector<Group>& groups);
    
    //static bool loadMTL(const string& path, map<string, Material>& materials);
    
private:
    static vector<string> split(const string& str, char delimiter);
    static string trim(const string& str);
    static string getDirectory(const string& filepath);

    static void parseFace(const string& faceStr, Face& face);
    static void parseVertice(const string& line, vector<glm::vec3>& vertices);
    static void parseTexCoord(const string& line, vector<glm::vec2>& texCoords);
    static void parseNormal(const string& line, vector<glm::vec3>& normals);
};

#endif