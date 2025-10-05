#pragma once
#include <string>

class Texture {
public:
    unsigned int id = 0;
    int width = 0;
    int height = 0;
    int channels = 0;
    
    bool loadFromFile(const std::string& path, bool flipVertically = true);
    void bind(unsigned int unit = 0) const;
    void unbind() const;
    
    ~Texture();
    
    // Create white default texture
    static unsigned int createWhiteTexture();
};