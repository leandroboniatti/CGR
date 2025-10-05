#include "OBJReader.h"
#include "Common.h"
#include <fstream>
#include <sstream>

OBJReader::OBJReader() {
}

OBJReader::~OBJReader() {
}

std::shared_ptr<OBJ3D> OBJReader::loadOBJ(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Erro: Não foi possível abrir o arquivo OBJ: " << filepath << std::endl;
        return nullptr;
    }

    auto obj = std::make_shared<OBJ3D>(filepath);
    std::shared_ptr<Group> currentGroup = std::make_shared<Group>("default");
    obj->addGroup(currentGroup);

    std::string line;
    std::string objDirectory = getDirectory(filepath);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            parseVertex(line, obj);
        }
        else if (prefix == "vt") {
            parseTextureCoord(line, obj);
        }
        else if (prefix == "vn") {
            parseNormal(line, obj);
        }
        else if (prefix == "f") {
            parseFace(line, currentGroup);
        }
        else if (prefix == "g" || prefix == "o") {
            currentGroup = parseGroup(line);
            obj->addGroup(currentGroup);
        }
        else if (prefix == "usemtl") {
            parseUseMaterial(line, currentGroup);
        }
        else if (prefix == "mtllib") {
            parseMaterialLib(line, objDirectory);
        }
    }

    file.close();
    
    obj->calculateBoundingBox();
    obj->setupVAOs();
    
    return obj;
}

bool OBJReader::loadMTL(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Aviso: Não foi possível abrir o arquivo MTL: " << filepath << std::endl;
        return false;
    }

    std::shared_ptr<Material> currentMaterial = nullptr;
    std::string line;
    std::string mtlDirectory = getDirectory(filepath);

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "newmtl") {
            parseMaterialName(line, currentMaterial);
        }
        else if (currentMaterial) {
            if (prefix == "Ka") {
                parseAmbient(line, currentMaterial);
            }
            else if (prefix == "Kd") {
                parseDiffuse(line, currentMaterial);
            }
            else if (prefix == "Ks") {
                parseSpecular(line, currentMaterial);
            }
            else if (prefix == "Ns") {
                parseShininess(line, currentMaterial);
            }
            else if (prefix == "map_Kd") {
                parseDiffuseTexture(line, currentMaterial, mtlDirectory);
            }
        }
    }

    file.close();
    return true;
}

const std::map<std::string, std::shared_ptr<Material>>& OBJReader::getMaterials() const {
    return materials;
}

void OBJReader::parseVertex(const std::string& line, std::shared_ptr<OBJ3D> obj) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 4) {
        glm::vec3 vertex = parseVec3(tokens);
        obj->addVertex(vertex);
    }
}

void OBJReader::parseTextureCoord(const std::string& line, std::shared_ptr<OBJ3D> obj) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 3) {
        glm::vec2 texCoord = parseVec2(tokens);
        obj->addTextureCoord(texCoord);
    }
}

void OBJReader::parseNormal(const std::string& line, std::shared_ptr<OBJ3D> obj) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 4) {
        glm::vec3 normal = parseVec3(tokens);
        obj->addNormal(normal);
    }
}

void OBJReader::parseFace(const std::string& line, std::shared_ptr<Group> currentGroup) {
    auto tokens = split(line, ' ');
    if (tokens.size() < 4) return; // Precisa de pelo menos 3 vértices

    Face face;
    
    for (size_t i = 1; i < tokens.size(); ++i) {
        auto indices = split(tokens[i], '/');
        
        if (!indices.empty()) {
            face.addVertexIndex(std::stoi(indices[0]));
        }
        if (indices.size() > 1 && !indices[1].empty()) {
            face.addTextureIndex(std::stoi(indices[1]));
        }
        if (indices.size() > 2 && !indices[2].empty()) {
            face.addNormalIndex(std::stoi(indices[2]));
        }
    }
    
    currentGroup->addFace(face);
}

std::shared_ptr<Group> OBJReader::parseGroup(const std::string& line) {
    auto tokens = split(line, ' ');
    std::string groupName = (tokens.size() > 1) ? tokens[1] : "default";
    return std::make_shared<Group>(groupName);
}

void OBJReader::parseUseMaterial(const std::string& line, std::shared_ptr<Group> currentGroup) {
    auto tokens = split(line, ' ');
    if (tokens.size() > 1) {
        auto it = materials.find(tokens[1]);
        if (it != materials.end()) {
            currentGroup->setMaterial(it->second);
        }
    }
}

void OBJReader::parseMaterialLib(const std::string& line, const std::string& objDirectory) {
    auto tokens = split(line, ' ');
    if (tokens.size() > 1) {
        std::string mtlPath = objDirectory + "/" + tokens[1];
        loadMTL(mtlPath);
    }
}

void OBJReader::parseMaterialName(const std::string& line, std::shared_ptr<Material>& currentMaterial) {
    auto tokens = split(line, ' ');
    if (tokens.size() > 1) {
        currentMaterial = std::make_shared<Material>(tokens[1]);
        materials[tokens[1]] = currentMaterial;
    }
}

void OBJReader::parseAmbient(const std::string& line, std::shared_ptr<Material> material) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 4) {
        glm::vec3 ambient = parseVec3(tokens);
        material->setAmbient(ambient);
    }
}

void OBJReader::parseDiffuse(const std::string& line, std::shared_ptr<Material> material) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 4) {
        glm::vec3 diffuse = parseVec3(tokens);
        material->setDiffuse(diffuse);
    }
}

void OBJReader::parseSpecular(const std::string& line, std::shared_ptr<Material> material) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 4) {
        glm::vec3 specular = parseVec3(tokens);
        material->setSpecular(specular);
    }
}

void OBJReader::parseShininess(const std::string& line, std::shared_ptr<Material> material) {
    auto tokens = split(line, ' ');
    if (tokens.size() >= 2) {
        float shininess = std::stof(tokens[1]);
        material->setShininess(shininess);
    }
}

void OBJReader::parseDiffuseTexture(const std::string& line, std::shared_ptr<Material> material, const std::string& mtlDirectory) {
    auto tokens = split(line, ' ');
    if (tokens.size() > 1) {
        std::string texturePath = mtlDirectory + "/" + tokens[1];
        material->setDiffuseTexturePath(texturePath);
    }
}

std::vector<std::string> OBJReader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(trim(token));
        }
    }
    
    return tokens;
}

std::string OBJReader::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

glm::vec3 OBJReader::parseVec3(const std::vector<std::string>& tokens, size_t startIndex) {
    glm::vec3 result(0.0f);
    if (tokens.size() > startIndex + 2) {
        result.x = std::stof(tokens[startIndex]);
        result.y = std::stof(tokens[startIndex + 1]);
        result.z = std::stof(tokens[startIndex + 2]);
    }
    return result;
}

glm::vec2 OBJReader::parseVec2(const std::vector<std::string>& tokens, size_t startIndex) {
    glm::vec2 result(0.0f);
    if (tokens.size() > startIndex + 1) {
        result.x = std::stof(tokens[startIndex]);
        result.y = std::stof(tokens[startIndex + 1]);
    }
    return result;
}

std::string OBJReader::getDirectory(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return filepath.substr(0, pos);
    }
    return ".";
}