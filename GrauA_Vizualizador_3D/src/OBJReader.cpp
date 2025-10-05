#include "OBJReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

bool OBJReader::loadOBJ(const std::string& path,
                       std::vector<glm::vec3>& vertices,
                       std::vector<glm::vec2>& texCoords,
                       std::vector<glm::vec3>& normals,
                       std::vector<Group>& groups,
                       std::map<std::string, Material>& materials) {
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }
    
    vertices.clear();
    texCoords.clear();
    normals.clear();
    groups.clear();
    
    std::string line;
    Group* currentGroup = nullptr;
    std::string currentMaterial = "";
    std::string objDirectory = getDirectory(path);
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "mtllib") {
            std::string mtlFile;
            iss >> mtlFile;
            std::string mtlPath = objDirectory + "/" + mtlFile;
            loadMTL(mtlPath, materials);
        }
        else if (prefix == "v") {
            parseVertex(line, vertices);
        }
        else if (prefix == "vt") {
            parseTexCoord(line, texCoords);
        }
        else if (prefix == "vn") {
            parseNormal(line, normals);
        }
        else if (prefix == "g" || prefix == "o") {
            std::string groupName;
            iss >> groupName;
            if (groupName.empty()) groupName = "default";
            
            groups.emplace_back(groupName);
            currentGroup = &groups.back();
            currentGroup->materialName = currentMaterial;
        }
        else if (prefix == "usemtl") {
            iss >> currentMaterial;
            if (currentGroup) {
                currentGroup->materialName = currentMaterial;
            }
        }
        else if (prefix == "f") {
            if (!currentGroup) {
                groups.emplace_back("default");
                currentGroup = &groups.back();
                currentGroup->materialName = currentMaterial;
            }
            
            Face face;
            parseFace(line, face);
            currentGroup->addFace(face);
        }
    }
    
    file.close();
    
    // Atribuir materiais aos grupos
    for (auto& group : groups) {
        if (!group.materialName.empty() && materials.find(group.materialName) != materials.end()) {
            group.material = materials[group.materialName];
            group.material.loadTextures();
        }
    }
    
    std::cout << "OBJ loaded: " << path << std::endl;
    std::cout << "Vertices: " << vertices.size() << ", TexCoords: " << texCoords.size() 
              << ", Normals: " << normals.size() << ", Groups: " << groups.size() << std::endl;
    
    return true;
}

bool OBJReader::loadMTL(const std::string& path, std::map<std::string, Material>& materials) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << path << std::endl;
        return false;
    }
    
    std::string line;
    Material* currentMaterial = nullptr;
    std::string mtlDirectory = getDirectory(path);
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "newmtl") {
            std::string materialName;
            iss >> materialName;
            materials[materialName] = Material(materialName);
            currentMaterial = &materials[materialName];
        }
        else if (currentMaterial) {
            if (prefix == "Ka") {
                iss >> currentMaterial->ambient.x >> currentMaterial->ambient.y >> currentMaterial->ambient.z;
            }
            else if (prefix == "Kd") {
                iss >> currentMaterial->diffuse.x >> currentMaterial->diffuse.y >> currentMaterial->diffuse.z;
            }
            else if (prefix == "Ks") {
                iss >> currentMaterial->specular.x >> currentMaterial->specular.y >> currentMaterial->specular.z;
            }
            else if (prefix == "Ns") {
                iss >> currentMaterial->shininess;
            }
            else if (prefix == "map_Kd") {
                std::string texturePath;
                iss >> texturePath;
                currentMaterial->diffuseTexture = mtlDirectory + "/" + texturePath;
            }
            else if (prefix == "map_Ks") {
                std::string texturePath;
                iss >> texturePath;
                currentMaterial->specularTexture = mtlDirectory + "/" + texturePath;
            }
            else if (prefix == "map_Bump" || prefix == "bump") {
                std::string texturePath;
                iss >> texturePath;
                currentMaterial->normalTexture = mtlDirectory + "/" + texturePath;
            }
        }
    }
    
    file.close();
    std::cout << "MTL loaded: " << path << " (" << materials.size() << " materials)" << std::endl;
    
    return true;
}

void OBJReader::parseFace(const std::string& faceStr, Face& face) {
    std::istringstream iss(faceStr);
    std::string prefix;
    iss >> prefix; // Skip "f"
    
    std::string vertexStr;
    while (iss >> vertexStr) {
        std::vector<std::string> indices = split(vertexStr, '/');
        
        if (!indices.empty() && !indices[0].empty()) {
            face.vertexIndices.push_back(std::stoi(indices[0]));
        }
        
        if (indices.size() > 1 && !indices[1].empty()) {
            face.textureIndices.push_back(std::stoi(indices[1]));
        }
        
        if (indices.size() > 2 && !indices[2].empty()) {
            face.normalIndices.push_back(std::stoi(indices[2]));
        }
    }
}

void OBJReader::parseVertex(const std::string& line, std::vector<glm::vec3>& vertices) {
    std::istringstream iss(line);
    std::string prefix;
    float x, y, z;
    iss >> prefix >> x >> y >> z;
    vertices.emplace_back(x, y, z);
}

void OBJReader::parseTexCoord(const std::string& line, std::vector<glm::vec2>& texCoords) {
    std::istringstream iss(line);
    std::string prefix;
    float u, v;
    iss >> prefix >> u >> v;
    texCoords.emplace_back(u, v);
}

void OBJReader::parseNormal(const std::string& line, std::vector<glm::vec3>& normals) {
    std::istringstream iss(line);
    std::string prefix;
    float x, y, z;
    iss >> prefix >> x >> y >> z;
    normals.emplace_back(x, y, z);
}

std::vector<std::string> OBJReader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string OBJReader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string OBJReader::getDirectory(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return filepath.substr(0, pos);
    }
    return ".";
}