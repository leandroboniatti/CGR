#include "Model.h"
#include "Texture.h"
#include <glad/gl.h>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

struct TempMeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    MaterialInfo material;
    std::unordered_map<std::string, unsigned int> dedup; // key v/vt/vn
};

static bool loadMTL(const std::string& mtlPath, std::unordered_map<std::string, MaterialInfo>& outMaterials) {
    std::ifstream f(mtlPath);
    if(!f) return false;
    std::string line, currentName;
    while(std::getline(f,line)) {
        std::istringstream iss(line);
        std::string tok; if(!(iss>>tok)) continue;
        if(tok=="newmtl") { iss>>currentName; outMaterials[currentName]=MaterialInfo{}; }
        else if(tok=="Ka" && !currentName.empty()) { iss>>outMaterials[currentName].Ka.r>>outMaterials[currentName].Ka.g>>outMaterials[currentName].Ka.b; }
        else if(tok=="Kd" && !currentName.empty()) { iss>>outMaterials[currentName].Kd.r>>outMaterials[currentName].Kd.g>>outMaterials[currentName].Kd.b; }
        else if(tok=="Ks" && !currentName.empty()) { iss>>outMaterials[currentName].Ks.r>>outMaterials[currentName].Ks.g>>outMaterials[currentName].Ks.b; }
        else if(tok=="Ns" && !currentName.empty()) { iss>>outMaterials[currentName].Ns; }
        else if(tok=="map_Kd" && !currentName.empty()) {
            std::string tex; iss>>tex; outMaterials[currentName].diffuseTex = TextureLoader::loadTexture((fs::path(mtlPath).parent_path()/tex).string(), true, false);
        }
    }
    return true;
}

bool Model::loadOBJ(const std::string& path, bool verbose) {
    m_meshes.clear();
    m_directory = fs::path(path).parent_path().string();
    std::ifstream f(path);
    if(!f) { std::cerr << "[OBJ] Falha ao abrir: " << path << '\n'; return false; }

    std::vector<glm::vec3> positions; positions.reserve(1024);
    std::vector<glm::vec2> texcoords; texcoords.reserve(1024);
    std::vector<glm::vec3> normals; normals.reserve(1024);
    std::unordered_map<std::string, MaterialInfo> materials;
    std::unordered_map<std::string, TempMeshData> meshGroups; // key material name
    std::string activeMaterial = "__default";
    meshGroups[activeMaterial];

    std::string line;
    while(std::getline(f,line)) {
        if(line.empty() || line[0]=='#') continue;
        std::istringstream iss(line); std::string tok; iss>>tok; if(tok.empty()) continue;
        if(tok=="v") {
            glm::vec3 p; iss>>p.x>>p.y>>p.z; positions.push_back(p);
        } else if(tok=="vt") {
            glm::vec2 uv; iss>>uv.x>>uv.y; texcoords.push_back(uv);
        } else if(tok=="vn") {
            glm::vec3 n; iss>>n.x>>n.y>>n.z; normals.push_back(n);
        } else if(tok=="f") {
            std::vector<std::string> verts; std::string vstr;
            while(iss>>vstr) verts.push_back(vstr);
            if(verts.size()<3) continue;
            // Triangulação fan
            auto& tmesh = meshGroups[activeMaterial];
            auto addVertex = [&](const std::string& key)->unsigned int {
                auto it=tmesh.dedup.find(key); if(it!=tmesh.dedup.end()) return it->second;
                int vi=-1,ti=-1,ni=-1; char c; std::stringstream ks(key); // parse v/t/n
                ks>>vi; if(ks.peek()=='/') { ks>>c; if(ks.peek()!='/'){ ks>>ti; } if(ks.peek()=='/') { ks>>c; ks>>ni; } }
                Vertex vert{};
                if(vi>0 && vi<= (int)positions.size()) vert.position = positions[vi-1];
                if(ti>0 && ti<= (int)texcoords.size()) vert.texcoord = texcoords[ti-1];
                if(ni>0 && ni<= (int)normals.size()) vert.normal = normals[ni-1];
                unsigned int newIndex = (unsigned int)tmesh.vertices.size();
                tmesh.vertices.push_back(vert);
                tmesh.dedup[key]=newIndex;
                return newIndex;
            };
            auto idx0 = addVertex(verts[0]);
            for(size_t i=1;i+1<verts.size();++i) {
                unsigned int i1 = addVertex(verts[i]);
                unsigned int i2 = addVertex(verts[i+1]);
                tmesh.indices.push_back(idx0);
                tmesh.indices.push_back(i1);
                tmesh.indices.push_back(i2);
            }
        } else if(tok=="usemtl") {
            std::string name; iss>>name; if(name.empty()) name="__default"; activeMaterial=name; meshGroups[activeMaterial];
        } else if(tok=="mtllib") {
            std::string mfile; iss>>mfile; if(!mfile.empty()) {
                loadMTL((fs::path(m_directory)/mfile).string(), materials);
            }
        }
    }

    // Transferir para Mesh + OpenGL
    for(auto & kv : meshGroups) {
        auto &name = kv.first; auto &tdata = kv.second;
        if(tdata.indices.empty()) continue; // ignora grupos vazios
        Mesh mesh; mesh.indexCount = tdata.indices.size();
        if(auto it = materials.find(name); it!=materials.end()) mesh.material = it->second; else mesh.material = MaterialInfo{};
        glGenVertexArrays(1,&mesh.vao);
        glGenBuffers(1,&mesh.vbo);
        glGenBuffers(1,&mesh.ebo);
        glBindVertexArray(mesh.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, tdata.vertices.size()*sizeof(Vertex), tdata.vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, tdata.indices.size()*sizeof(unsigned int), tdata.indices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
        glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,normal));
        glEnableVertexAttribArray(2); glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,texcoord));
        glBindVertexArray(0);
        m_meshes.push_back(mesh);
    }
    if(verbose) std::cout << "[OBJ] Carregado (parser interno): " << path << " meshes=" << m_meshes.size() << '\n';
    return !m_meshes.empty();
}
