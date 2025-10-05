#pragma once
#include <string>
#include <glad/glad.h>

class ShaderProgram {
private:
    unsigned int programID;
    
    std::string readShaderFile(const std::string& filePath);
    unsigned int compileShader(const std::string& source, GLenum shaderType);
    bool checkCompileErrors(unsigned int shader, const std::string& type);

public:
    ShaderProgram();
    ~ShaderProgram();
    
    bool loadShaders(const std::string& vertexPath, const std::string& fragmentPath);
    void use();
    unsigned int getID() const { return programID; }
    
    // Utility functions for setting uniforms
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const float* value) const;
};