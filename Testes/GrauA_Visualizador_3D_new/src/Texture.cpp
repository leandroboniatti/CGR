#include "Texture.h"
#include "Common.h"

Texture::Texture() 
    : textureID(0)
    , width(0)
    , height(0)
    , channels(0)
    , format(GL_RGB)
    , loaded(false) {
}

Texture::~Texture() {
    cleanup();
}

bool Texture::loadFromFile(const std::string& path) {
    filepath = path;
    
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        std::cerr << "Erro: Falha ao carregar textura: " << path << std::endl;
        return false;
    }
    
    bool success = loadFromData(data, width, height, channels);
    stbi_image_free(data);
    
    return success;
}

bool Texture::loadFromData(unsigned char* data, int w, int h, int ch) {
    if (!data) return false;
    
    width = w;
    height = h;
    channels = ch;
    format = determineFormat(channels);
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // Configurações padrão
    setWrapMode(GL_REPEAT, GL_REPEAT);
    setFilterMode(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    generateMipmaps();
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    loaded = true;
    return true;
}

void Texture::bind(unsigned int unit) const {
    if (loaded) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getID() const {
    return textureID;
}

int Texture::getWidth() const {
    return width;
}

int Texture::getHeight() const {
    return height;
}

int Texture::getChannels() const {
    return channels;
}

const std::string& Texture::getFilepath() const {
    return filepath;
}

bool Texture::isLoaded() const {
    return loaded;
}

void Texture::setWrapMode(GLenum wrapS, GLenum wrapT) {
    if (loaded) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::setFilterMode(GLenum minFilter, GLenum magFilter) {
    if (loaded) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::generateMipmaps() {
    if (loaded) {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Texture::cleanup() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
        loaded = false;
    }
}

GLenum Texture::determineFormat(int channels) {
    switch (channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}