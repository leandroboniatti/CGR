#include "OBJReader.h"
#include <iostream>
#include <map>

Obj3D* OBJReader::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo OBJ: " << filename << std::endl;
        return nullptr;
    }
    
    Obj3D* obj = new Obj3D();
    obj->mesh = new Mesh();
    
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_texCoords;
    std::vector<glm::vec3> temp_normals;
    
    Group currentGroup("default");
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        
        if (prefix == "v") {
            // Vértice
            glm::vec3 vertex = parseVec3(line);
            temp_vertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            // Coordenada de textura
            glm::vec2 texCoord = parseVec2(line);
            temp_texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            // Normal
            glm::vec3 normal = parseVec3(line);
            temp_normals.push_back(normal);
        }
        else if (prefix == "f") {
            // Face
            std::vector<int> faceData = parseFace(line);
            
            // Quebrar face em triângulos se necessário
            std::vector<std::vector<int>> triangles = triangulateFace(faceData);
            
            for (const auto& triangle : triangles) {
                Face face;
                for (size_t i = 0; i < triangle.size(); i += 3) {
                    int vIndex = triangle[i] - 1;  // OBJ usa indexação 1-based
                    int tIndex = (i + 1 < triangle.size()) ? triangle[i + 1] - 1 : -1;
                    int nIndex = (i + 2 < triangle.size()) ? triangle[i + 2] - 1 : -1;
                    
                    face.addVertex(vIndex, tIndex, nIndex);
                }
                currentGroup.addFace(face);
            }
        }
        else if (prefix == "g") {
            // Novo grupo
            if (!currentGroup.faces.empty()) {
                obj->mesh->addGroup(currentGroup);
            }
            std::string groupName;
            iss >> groupName;
            currentGroup = Group(groupName);
        }
    }
    
    // Adicionar último grupo
    if (!currentGroup.faces.empty()) {
        obj->mesh->addGroup(currentGroup);
    }
    
    // Copiar dados temporários para o mesh
    for (const auto& vertex : temp_vertices) {
        obj->mesh->addVertex(vertex);
    }
    for (const auto& texCoord : temp_texCoords) {
        obj->mesh->addTexCoord(texCoord);
    }
    for (const auto& normal : temp_normals) {
        obj->mesh->addNormal(normal);
    }
    
    obj->mesh->setupBuffers();
    obj->mesh->calculateBounds();
    obj->updateTransform();
    
    file.close();
    return obj;
}

std::vector<std::string> OBJReader::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    
    while (std::getline(stream, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

glm::vec3 OBJReader::parseVec3(const std::string& line) {
    std::istringstream iss(line);
    std::string prefix;
    float x, y, z;
    
    iss >> prefix >> x >> y >> z;
    return glm::vec3(x, y, z);
}

glm::vec2 OBJReader::parseVec2(const std::string& line) {
    std::istringstream iss(line);
    std::string prefix;
    float x, y;
    
    iss >> prefix >> x >> y;
    return glm::vec2(x, y);
}

std::vector<int> OBJReader::parseFace(const std::string& line) {
    std::vector<int> indices;
    std::istringstream iss(line);
    std::string prefix, vertex;
    
    iss >> prefix; // Skip 'f'
    
    while (iss >> vertex) {
        std::vector<std::string> parts = split(vertex, '/');
        
        // Vertex index
        if (!parts.empty() && !parts[0].empty()) {
            indices.push_back(std::stoi(parts[0]));
        }
        
        // Texture coordinate index
        if (parts.size() > 1 && !parts[1].empty()) {
            indices.push_back(std::stoi(parts[1]));
        } else {
            indices.push_back(0);
        }
        
        // Normal index
        if (parts.size() > 2 && !parts[2].empty()) {
            indices.push_back(std::stoi(parts[2]));
        } else {
            indices.push_back(0);
        }
    }
    
    return indices;
}

std::vector<std::vector<int>> OBJReader::triangulateFace(const std::vector<int>& face) {
    std::vector<std::vector<int>> triangles;
    
    // Se já é um triângulo, retorna como está
    if (face.size() == 9) { // 3 vértices * 3 índices cada
        triangles.push_back(face);
        return triangles;
    }
    
    // Triangular face com fan triangulation
    int numVertices = face.size() / 3;
    for (int i = 1; i < numVertices - 1; ++i) {
        std::vector<int> triangle;
        
        // Primeiro vértice
        triangle.push_back(face[0]);
        triangle.push_back(face[1]);
        triangle.push_back(face[2]);
        
        // Vértice i
        triangle.push_back(face[i * 3]);
        triangle.push_back(face[i * 3 + 1]);
        triangle.push_back(face[i * 3 + 2]);
        
        // Vértice i+1
        triangle.push_back(face[(i + 1) * 3]);
        triangle.push_back(face[(i + 1) * 3 + 1]);
        triangle.push_back(face[(i + 1) * 3 + 2]);
        
        triangles.push_back(triangle);
    }
    
    return triangles;
}