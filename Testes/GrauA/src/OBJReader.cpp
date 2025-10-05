#include "OBJReader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string OBJReader::getDirectory(const std::string& filepath) {
    size_t lastSlash = filepath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return filepath.substr(0, lastSlash + 1);
    }
    return "";
}

bool OBJReader::loadOBJ(const std::string& filepath, Mesh& mesh, std::unordered_map<std::string, Material>& materials) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
        return false;
    }
    
    std::string directory = getDirectory(filepath);
    std::string line;
    Group* currentGroup = nullptr;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "mtllib") {
            std::string mtlFile;
            iss >> mtlFile;
            mesh.mtllib = mtlFile;
            loadMTL(directory + mtlFile, materials);
        }
        else if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            mesh.vertices.push_back(vertex);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            mesh.normals.push_back(normal);
        }
        else if (prefix == "vt") {
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            mesh.texCoords.push_back(texCoord);
        }
        else if (prefix == "g" || prefix == "o") {
            std::string groupName;
            iss >> groupName;
            mesh.groups.emplace_back();
            currentGroup = &mesh.groups.back();
            currentGroup->name = groupName;
        }
        else if (prefix == "usemtl") {
            std::string materialName;
            iss >> materialName;
            if (currentGroup) {
                currentGroup->material = materialName;
            }
        }
        else if (prefix == "f") {
            if (!currentGroup) {
                mesh.groups.emplace_back();
                currentGroup = &mesh.groups.back();
                currentGroup->name = "default";
            }
            
            Face face;
            std::string vertex;
            
            while (iss >> vertex) {
                std::istringstream vertexStream(vertex);
                std::string index;
                
                // Parse v/vt/vn format
                if (std::getline(vertexStream, index, '/')) {
                    face.verts.push_back(std::stoi(index) - 1); // OBJ indices are 1-based
                }
                
                if (std::getline(vertexStream, index, '/')) {
                    if (!index.empty()) {
                        face.texts.push_back(std::stoi(index) - 1);
                    }
                }
                
                if (std::getline(vertexStream, index)) {
                    if (!index.empty()) {
                        face.norms.push_back(std::stoi(index) - 1);
                    }
                }
            }
            
            currentGroup->faces.push_back(face);
        }
    }
    
    // Calculate bounding box
    mesh.calculateBounds();
    
    // Setup OpenGL data for each group
    for (auto& group : mesh.groups) {
        group.setupGL(mesh.vertices, mesh.normals, mesh.texCoords);
    }
    
    return true;
}

bool OBJReader::loadMTL(const std::string& filepath, std::unordered_map<std::string, Material>& materials) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open MTL file: " << filepath << std::endl;
        return false;
    }
    
    std::string directory = getDirectory(filepath);
    std::string line;
    Material* currentMaterial = nullptr;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "newmtl") {
            std::string materialName;
            iss >> materialName;
            materials[materialName] = Material();
            currentMaterial = &materials[materialName];
            currentMaterial->name = materialName;
        }
        else if (currentMaterial) {
            if (prefix == "Ka") {
                iss >> currentMaterial->Ka.r >> currentMaterial->Ka.g >> currentMaterial->Ka.b;
            }
            else if (prefix == "Kd") {
                iss >> currentMaterial->Kd.r >> currentMaterial->Kd.g >> currentMaterial->Kd.b;
            }
            else if (prefix == "Ks") {
                iss >> currentMaterial->Ks.r >> currentMaterial->Ks.g >> currentMaterial->Ks.b;
            }
            else if (prefix == "Ns") {
                iss >> currentMaterial->Ns;
            }
            else if (prefix == "map_Kd") {
                std::string texturePath;
                iss >> texturePath;
                currentMaterial->mapKd = directory + texturePath;
            }
        }
    }
    
    return true;
}