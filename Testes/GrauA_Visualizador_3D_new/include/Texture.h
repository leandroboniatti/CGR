#pragma once

#include <string>
#include <glad/glad.h>

class Texture {
private:
    unsigned int textureID;
    int width;
    int height;
    int channels;
    GLenum format;
    bool loaded;
    std::string filepath;

public:
    Texture();
    ~Texture();

    // Carregamento
    bool loadFromFile(const std::string& path);
    bool loadFromData(unsigned char* data, int w, int h, int ch);

    // Uso
    void bind(unsigned int unit = 0) const;
    void unbind() const;

    // Getters
    unsigned int getID() const;
    int getWidth() const;
    int getHeight() const;
    int getChannels() const;
    const std::string& getFilepath() const;
    bool isLoaded() const;

    // Configurações
    void setWrapMode(GLenum wrapS, GLenum wrapT);
    void setFilterMode(GLenum minFilter, GLenum magFilter);
    void generateMipmaps();

    // Cleanup
    void cleanup();

private:
    GLenum determineFormat(int channels);
};