#pragma once

#include <string>
#include <glad/glad.h>

class Texture {
public:
    unsigned int ID;
    std::string type;
    std::string path;
    
    Texture();
    Texture(const std::string& path, const std::string& type = "diffuse");
    ~Texture();
    
    bool loadFromFile(const std::string& path);
    void bind(unsigned int unit = 0);
    void unbind();
    
private:
    int width, height, nrChannels;
};