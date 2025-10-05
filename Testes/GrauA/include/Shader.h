#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int programId = 0;
    
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;
    void setVec3(const std::string& name, const glm::vec3& vector) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    
    ~Shader();
private:
    unsigned int compileShader(const std::string& source, unsigned int type);
    std::string loadFile(const std::string& path);
};