#include "OBJReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

// Realiza a leitura de um arquivo OBJ, preenchendo os vetores passados por referência
// Uso: System::LoadSceneObjects -> OBJ3D:loadObject -> Mesh::readObjectModel -> OBJReader::readFileOBJ
// As classes anteriores "chamam" este método para carregar o modelo OBJ
// Os vetores de armazenamento de vértices, textura, normais e grupos
// são passados como referência para este método, que os preenche
bool OBJReader::readFileOBJ(const string& path,
                            vector<glm::vec3>& vertices,
                            vector<glm::vec2>& texCoords,
                            vector<glm::vec3>& normals,
                            vector<Group>& groups)          {

    ifstream objFile(path);    // Abre o arquivo OBJ para leitura

    if (!objFile.is_open()) {  // Debug
        cerr << "Falha ao abrir arquivo OBJ: " << path << endl;
        return false;
    }
    
    vertices.clear();
    texCoords.clear();
    normals.clear();
    groups.clear();
    
    string line;  // variável temporária para armazenar cada linha lida do arquivo de configuração
    Group* currentGroup = nullptr;  // Ponteiro para o grupo em processamento

    string objDirectory = getDirectory(path);

    while (getline(objFile, line)) { // Lê o arquivo linha por linha e armazena cada linha em 'line'
        line = trim(line);  // Remove espaços em branco no início e no final da linha

        if (line.empty() || line[0] == '#') { continue; } // Ignora linhas vazias e de comentários "#"

        istringstream sline(line); // Cria um stream de string para processar a linha
        string prefix;
        sline >> prefix;  // Lê o prefixo da linha (v, vt, vn, f, etc.)

        if (prefix == "mtllib") { // ignora - não implementado
            continue;
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
            string groupName;
            sline >> groupName;
            if (groupName.empty()) groupName = "default";
            groups.emplace_back(groupName); // adiciona o grupo em processamento ao vetor de grupos
            currentGroup = &groups.back();  // ponteiro para o último elemento do vetor de groups (grupo atual)
        }
        else if (prefix == "usemtl") { // ignora - não implementado
            continue;
        }
        else if (prefix == "f") {   // Adiciona os indices que referenciam os vértices de uma face ao grupo atual.
                                    // No arquivo .obj cada vertice é representado por índices no formato:
                                    // vertice/texCoord/normal -> ex.: f 1/1/1 2/2/1 3/3/1 4/4/1
            if (!currentGroup) {
                groups.emplace_back("default"); // adiciona o grupo em processamento ao vetor de grupos
                currentGroup = &groups.back();  // ponteiro para o último elemento do vetor de groups (grupo atual)
            }
            
            Face face;  // gera/instancia uma nova face
            parseFace(line, face);  // popula a face com os INDICES lidos na linha
            currentGroup->addFace(face); // adiciona a face processada ao grupo atual, que já está no vetor de grupos
        }
    }

    objFile.close();    // Fecha o arquivo .obj

    return true;
}

// Analisa uma linha do arquivo OBJ e preenche os indices da face
// nos vetores de índices da face (vertexIndices, textureIndices, normalIndices)
void OBJReader::parseFace(const string& faceStr, Face& face) {
    
    istringstream sline(faceStr); // Cria um stream de string para processar a linha

    string indicesStr;  // string temporária para processamento

    sline >> indicesStr; // ignora o caracter "f"

    while (sline >> indicesStr) {  // para cada sequencia de indices presentes na linha:

        vector<string> indices = split(indicesStr, '/');       // retira o caracter "/"

        if (!indices.empty() && !indices[0].empty()) {         // Adiciona o índice que referencia as coordenadas do vértice (vec3)
            face.vertexIndices.push_back(stoi(indices[0]));    // presentes no vetor de vértices definido na classe Mesh
        }                                                      // (std::vector<glm::vec3> vertices;)

        if (indices.size() > 1 && !indices[1].empty()) {       // Adiciona o índice que referencia as coordenadas de textura (vec2)
            face.textureIndices.push_back(stoi(indices[1]));   // presentes no vetor de coordenadas de textura definido na classe Mesh
        }                                                      // (std::vector<glm::vec2> texCoords;)

        if (indices.size() > 2 && !indices[2].empty()) {       // Adiciona o índice que referencia as coordenadas da normal (vec3)
            face.normalIndices.push_back(stoi(indices[2]));    // presentes no vetor de normais definido na classe Mesh
        }                                                      // (std::vector<glm::vec3> normals;)
    }
}


void OBJReader::parseVertice(const string& line, vector<glm::vec3>& vertices) {
    
    istringstream sline(line);
    string prefix;
    float x, y, z;
    sline >> prefix >> x >> y >> z;
    vertices.emplace_back(x, y, z);
}


void OBJReader::parseTexCoord(const string& line, vector<glm::vec2>& texCoords) {

    istringstream sline(line);
    string prefix;
    float u, v;
    sline >> prefix >> u >> v;
    texCoords.emplace_back(u, v);
}


void OBJReader::parseNormal(const string& line, vector<glm::vec3>& normals) {

    istringstream sline(line);
    string prefix;
    float x, y, z;
    sline >> prefix >> x >> y >> z;
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