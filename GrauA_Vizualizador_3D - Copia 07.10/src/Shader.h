#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader {
public:
    unsigned int ID;
    
    Shader();
    Shader(const string& vertexPath, const string& fragmentPath);
    ~Shader();

    bool loadFromFiles(const string& vertexPath, const string& fragmentPath);
    bool loadFromStrings(const string& vertexSource, const string& fragmentSource);

    void use() const;
    
    // Funções utilitárias para uniformes
    void setBool (const std::string& name, bool value) const;
    void setInt  (const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2 (const std::string& name, const glm::vec2& value) const;
    void setVec2 (const std::string& name, float x, float y) const;
    void setVec3 (const std::string& name, const glm::vec3& value) const;
    void setVec3 (const std::string& name, float x, float y, float z) const;
    void setVec4 (const std::string& name, const glm::vec4& value) const;
    void setVec4 (const std::string& name, float x, float y, float z, float w) const;
    void setMat2 (const std::string& name, const glm::mat2& mat) const;
    void setMat3 (const std::string& name, const glm::mat3& mat) const;
    void setMat4 (const std::string& name, const glm::mat4& mat) const;
    
private:
    string readFile(const string& filePath) const;
    unsigned int compileShader(const string& source, GLenum shaderType) const;
    bool checkCompileErrors(unsigned int shader, const string& type) const;
    void cleanup();
};

#endif