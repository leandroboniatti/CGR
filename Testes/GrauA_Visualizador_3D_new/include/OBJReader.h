#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "OBJ3D.h"
#include "Material.h"

class OBJReader {
private:
    std::map<std::string, std::shared_ptr<Material>> materials;

public:
    OBJReader();
    ~OBJReader();

    // Método principal para carregar arquivo OBJ
    std::shared_ptr<OBJ3D> loadOBJ(const std::string& filepath);

    // Carregamento de materiais
    bool loadMTL(const std::string& filepath);

    // Getters
    const std::map<std::string, std::shared_ptr<Material>>& getMaterials() const;

private:
    // Métodos auxiliares para parsing
    void parseVertex(const std::string& line, std::shared_ptr<OBJ3D> obj);
    void parseTextureCoord(const std::string& line, std::shared_ptr<OBJ3D> obj);
    void parseNormal(const std::string& line, std::shared_ptr<OBJ3D> obj);
    void parseFace(const std::string& line, std::shared_ptr<Group> currentGroup);
    std::shared_ptr<Group> parseGroup(const std::string& line);
    void parseUseMaterial(const std::string& line, std::shared_ptr<Group> currentGroup);
    void parseMaterialLib(const std::string& line, const std::string& objDirectory);

    // Parsing de materiais
    void parseMaterialName(const std::string& line, std::shared_ptr<Material>& currentMaterial);
    void parseAmbient(const std::string& line, std::shared_ptr<Material> material);
    void parseDiffuse(const std::string& line, std::shared_ptr<Material> material);
    void parseSpecular(const std::string& line, std::shared_ptr<Material> material);
    void parseShininess(const std::string& line, std::shared_ptr<Material> material);
    void parseDiffuseTexture(const std::string& line, std::shared_ptr<Material> material, const std::string& mtlDirectory);

    // Utilitários
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string trim(const std::string& str);
    glm::vec3 parseVec3(const std::vector<std::string>& tokens, size_t startIndex = 1);
    glm::vec2 parseVec2(const std::vector<std::string>& tokens, size_t startIndex = 1);
    std::string getDirectory(const std::string& filepath);
};