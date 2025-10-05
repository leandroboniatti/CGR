#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

class Shader {
private:
    unsigned int programID;
    bool compiled;

public:
    Shader();
    ~Shader();

    // Carregamento e compilação
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool loadFromStrings(const std::string& vertexSource, const std::string& fragmentSource);
    
    // Uso do shader
    void use() const;
    void unuse() const;
    
    // Getters
    unsigned int getProgramID() const;
    bool isCompiled() const;

    // Uniforms - int
    void setUniform(const std::string& name, int value) const;
    void setUniform(const std::string& name, bool value) const;
    
    // Uniforms - float
    void setUniform(const std::string& name, float value) const;
    void setUniform(const std::string& name, float x, float y) const;
    void setUniform(const std::string& name, float x, float y, float z) const;
    void setUniform(const std::string& name, float x, float y, float z, float w) const;
    
    // Uniforms - glm
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat3& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

    // Cleanup
    void cleanup();

private:
    // Métodos auxiliares
    unsigned int compileShader(const std::string& source, GLenum shaderType);
    bool linkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    std::string readFile(const std::string& filepath);
    void checkCompileErrors(unsigned int shader, const std::string& type);
    int getUniformLocation(const std::string& name) const;
};