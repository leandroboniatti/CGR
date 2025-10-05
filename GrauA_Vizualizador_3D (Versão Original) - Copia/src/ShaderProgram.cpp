#include "ShaderProgram.h"
#include <iostream>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram() : programID(0) {}

ShaderProgram::~ShaderProgram() {
    if (programID) {
        glDeleteProgram(programID);
    }
}

bool ShaderProgram::loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    // Ler arquivos de shader
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        return false;
    }
    
    // Compilar shaders
    unsigned int vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    unsigned int fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);
    
    if (vertex == 0 || fragment == 0) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return false;
    }
    
    // Criar programa de shader
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    
    // Verificar erros de linkagem
    int success;
    char infoLog[1024];
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        std::cerr << "Erro de linkagem do programa de shader:\n" << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }
    
    // Deletar shaders (já estão linkados ao programa)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return true;
}

std::string ShaderProgram::readShaderFile(const std::string& filePath) {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "Erro ao ler arquivo de shader: " << filePath << std::endl;
        return "";
    }
}

unsigned int ShaderProgram::compileShader(const std::string& source, GLenum shaderType) {
    unsigned int shader = glCreateShader(shaderType);
    const char* shaderCode = source.c_str();
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    
    if (!checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT")) {
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

bool ShaderProgram::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cerr << "Erro de compilação do shader " << type << ":\n" << infoLog << std::endl;
        return false;
    }
    
    return true;
}

void ShaderProgram::use() {
    glUseProgram(programID);
}

void ShaderProgram::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void ShaderProgram::setMat4(const std::string& name, const float* value) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, value);
}