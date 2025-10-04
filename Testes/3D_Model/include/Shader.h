#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
    Shader() = default;
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    bool load(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void reload();

    // Uniform helpers
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    unsigned int id() const { return m_program; }

private:
    std::string m_vertexPath;
    std::string m_fragmentPath;
    unsigned int m_program = 0;
    mutable std::unordered_map<std::string, int> m_locationCache;

    int getLocation(const std::string& name) const;
    std::string loadFile(const std::string& path);
    unsigned int compileStage(unsigned int type, const std::string& src);
    bool link(unsigned int vs, unsigned int fs);
    void destroy();
};
