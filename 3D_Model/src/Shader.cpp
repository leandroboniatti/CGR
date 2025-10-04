#include "Shader.h"
#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    load(vertexPath, fragmentPath);
}

Shader::~Shader() { destroy(); }

void Shader::destroy() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

std::string Shader::loadFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        std::cerr << "[Shader] Falha ao abrir arquivo: " << path << '\n';
        return {};
    }
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

unsigned int Shader::compileStage(unsigned int type, const std::string& src) {
    unsigned int id = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(id, 1, &c, nullptr);
    glCompileShader(id);
    int success; glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024]; glGetShaderInfoLog(id, 1024, nullptr, log);
        std::cerr << "[Shader] Erro de compilacao: " << log << '\n';
    }
    return id;
}

bool Shader::link(unsigned int vs, unsigned int fs) {
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    int success; glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024]; glGetProgramInfoLog(m_program, 1024, nullptr, log);
        std::cerr << "[Shader] Erro de linkagem: " << log << '\n';
        return false;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    m_vertexPath = vertexPath; m_fragmentPath = fragmentPath;
    destroy();
    std::string vsrc = loadFile(vertexPath);
    std::string fsrc = loadFile(fragmentPath);
    if (vsrc.empty() || fsrc.empty()) return false;
    unsigned int vs = compileStage(GL_VERTEX_SHADER, vsrc);
    unsigned int fs = compileStage(GL_FRAGMENT_SHADER, fsrc);
    bool ok = link(vs, fs);
    if (ok) std::cout << "[Shader] Compilado: " << vertexPath << ", " << fragmentPath << '\n';
    return ok;
}

void Shader::use() const { glUseProgram(m_program); }

void Shader::reload() { load(m_vertexPath, m_fragmentPath); }

int Shader::getLocation(const std::string& name) const {
    if (auto it = m_locationCache.find(name); it != m_locationCache.end()) return it->second;
    int loc = glGetUniformLocation(m_program, name.c_str());
    m_locationCache[name] = loc;
    return loc;
}

void Shader::setBool(const std::string& name, bool value) const { glUniform1i(getLocation(name), (int)value); }
void Shader::setInt(const std::string& name, int value) const { glUniform1i(getLocation(name), value); }
void Shader::setFloat(const std::string& name, float value) const { glUniform1f(getLocation(name), value); }
void Shader::setVec3(const std::string& name, const glm::vec3& value) const { glUniform3fv(getLocation(name), 1, &value[0]); }
void Shader::setMat4(const std::string& name, const glm::mat4& value) const { glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(value)); }
