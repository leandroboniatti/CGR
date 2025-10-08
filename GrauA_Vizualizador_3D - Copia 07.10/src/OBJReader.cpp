#include "OBJReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Usado em Mesh::loadFromOBJ
// A classe Mesh chama este método para carregar o modelo OBJ
// Na classe Mesh é que são definidos os vetores de armazenamento
// de vértices, coordenadas de textura, normais, grupos e materiais
// que são passados como referência para este método
bool OBJReader::loadOBJ(const string& path,
                        vector<glm::vec3>& vertices,
                        vector<glm::vec2>& texCoords,
                        vector<glm::vec3>& normals,
                        vector<Group>& groups,
                        map<string, Material>& materials) {

    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open OBJ file: " << path << endl;
        return false;
    }
    
    vertices.clear();
    texCoords.clear();
    normals.clear();
    groups.clear();
    
    string line;
    Group* currentGroup = nullptr;  // Ponteiro para o grupo em processamento
    string currentMaterial = "";
    string objDirectory = getDirectory(path);

    while (getline(file, line)) { // Lê o arquivo linha por linha
        line = trim(line);  // Remove espaços em branco no início e no final da linha

        if (line.empty() || line[0] == '#') { continue; } // Ignora linhas vazias e comentários "#"

        istringstream iss(line); // Cria um stream de string para processar a linha
        string prefix;
        iss >> prefix;  // Lê o prefixo da linha (v, vt, vn, f, etc.)
        
        if (prefix == "mtllib") {
            string mtlFile;
            iss >> mtlFile;
            string mtlPath = objDirectory + "/" + mtlFile;
            loadMTL(mtlPath, materials);    // Carrega o arquivo MTL e popula o mapa de materiais
        }
        else if (prefix == "v") {           // adiciona as coord. do vértice (vec3), presentes na linha, ao vetor de
            parseVertice(line, vertices);   // coordenadas dos vértices definido na classe Mesh (std::vector<glm::vec3> vertices;)
        }
        else if (prefix == "vt") {          // adiciona as coord. de textura (vec2), presentes na linha, ao vetor de
            parseTexCoord(line, texCoords); // coordenadas de textura definido na classe Mesh (std::vector<glm::vec2> texCoords);
        }
        else if (prefix == "vn") {          // adiciona as normais (vec3), presentes na linha, ao vetor de    
            parseNormal(line, normals);     // normais definido na classe Mesh (std::vector<glm::vec3> normals);
        }
        else if (prefix == "g" || prefix == "o") {  // Verifica o nome do grupo e inicia um novo grupo
            std::string groupName;
            iss >> groupName;
            if (groupName.empty()) groupName = "default";
            groups.emplace_back(groupName);
            currentGroup = &groups.back(); // ponteiro para o último elemento do container groups
            currentGroup->materialName = currentMaterial;
        }
        else if (prefix == "usemtl") {  // Define o material a ser usado no grupo atual
            iss >> currentMaterial;
            if (!currentGroup) {
                groups.emplace_back("default");
                currentGroup = &groups.back();
                currentGroup->materialName = currentMaterial;
            }
            //if (currentGroup) {
            //    currentGroup->materialName = currentMaterial;
            //}
        }
        else if (prefix == "f") {   // Adiciona os indices que referenciam os vértices de uma face ao grupo atual
            if (!currentGroup) {    // cada vertice é representado por índices no formato vertice/texCoord/normal
                groups.emplace_back("default");
                currentGroup = &groups.back();
                currentGroup->materialName = currentMaterial;
            }
            
            Face face;  // gera uma nova face
            parseFace(line, face);  // popula a face com os índices lidos na linha
            currentGroup->addFace(face);
        }
    }
    
    file.close();

    // Atribuir materiais aos grupos, caso tenham sido definidos e o material exista no mapa
    for (auto& group : groups) {    // para cada grupo presente no vetor de grupos
        if (!group.materialName.empty() && materials.find(group.materialName) != materials.end()) {
            group.material = materials[group.materialName];
            group.material.loadTextures();
        }
    }

    //std::cout << "Arquivo OBJ carregado: " << path << std::endl;
    //std::cout << "nº de Vertices: " << vertices.size() << ", nº de TexCoords: " << texCoords.size()
    //          << ", nº de Normais: " << normals.size() << ", nº de Grupos: " << groups.size() << std::endl;

    return true;
}

bool OBJReader::loadMTL(const string& path, map<string, Material>& materials) {
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open MTL file: " << path << endl;
        return false;
    }

    string line;
    Material* currentMaterial = nullptr;
    string mtlDirectory = getDirectory(path);

    while (getline(file, line)) {
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

void OBJReader::parseFace(const string& faceStr, Face& face) {
    istringstream iss(faceStr); // Cria um stream de string para processar a linha
    string prefix;
    iss >> prefix; // ignora "f" 

    string indicesStr;
    //string vertexStr;
    while (iss >> indicesStr) {  // para cada sequencia de indices presentes na linha:
        vector<string> indices = split(indicesStr, '/');

        if (!indices.empty() && !indices[0].empty()) {              // Adiciona o índice que referencia as coordenadas do vértice (vec3)
            face.vertexIndices.push_back(stoi(indices[0]));    // presentes no vetor de vértices definido na classe Mesh
        }                                                           // (std::vector<glm::vec3> vertices;)

        if (indices.size() > 1 && !indices[1].empty()) {            // Adiciona o índice que referencia as coordenadas de textura (vec2)
            face.textureIndices.push_back(stoi(indices[1]));   // presentes no vetor de coordenadas de textura definido na classe Mesh
        }                                                           // (std::vector<glm::vec2> texCoords;)

        if (indices.size() > 2 && !indices[2].empty()) {            // Adiciona o índice que referencia as coordenadas da normal (vec3)
            face.normalIndices.push_back(stoi(indices[2]));    // presentes no vetor de normais definido na classe Mesh
        }                                                           // (std::vector<glm::vec3> normals;)
    }
}

void OBJReader::parseVertice(const string& line, vector<glm::vec3>& vertices) {
    istringstream iss(line);
    string prefix;
    float x, y, z;
    iss >> prefix >> x >> y >> z;
    vertices.emplace_back(x, y, z);
}

void OBJReader::parseTexCoord(const string& line, vector<glm::vec2>& texCoords) {
    std::istringstream iss(line);
    std::string prefix;
    float u, v;
    iss >> prefix >> u >> v;
    texCoords.emplace_back(u, v);
}

void OBJReader::parseNormal(const string& line, vector<glm::vec3>& normals) {
    istringstream iss(line);
    string prefix;
    float x, y, z;
    iss >> prefix >> x >> y >> z;
    normals.emplace_back(x, y, z);
}

vector<string> OBJReader::split(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

string OBJReader::trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

string OBJReader::getDirectory(const string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos != string::npos) {
        return filepath.substr(0, pos);
    }
    return ".";
}