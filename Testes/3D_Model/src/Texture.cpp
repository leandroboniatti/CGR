#include "Texture.h"
#include <glad/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <vector>

int TextureLoader::loadTexture(const std::string& path, bool flipVertical, bool srgb) {
    stbi_set_flip_vertically_on_load(flipVertical);
    int w,h,c; unsigned char* data = stbi_load(path.c_str(), &w,&h,&c, 0);
    if(!data){
        std::cerr << "[Texture] Falha ao carregar: " << path << '\n';
        return whiteTexture();
    }
    GLenum format = c==1?GL_RED:c==3?GL_RGB:GL_RGBA;
    GLuint tex; glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLenum internalFormat = format;
    if (srgb && (format==GL_RGB || format==GL_RGBA)) {
        internalFormat = (format==GL_RGB)?0x8C41:0x8C43; // GL_SRGB8 / GL_SRGB8_ALPHA8
    }
    glTexImage2D(GL_TEXTURE_2D,0,internalFormat,w,h,0,format,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return (int)tex;
}

int TextureLoader::whiteTexture() {
    static int tex = -1;
    if (tex != -1) return tex;
    unsigned char white[4] = {255,255,255,255};
    GLuint id; glGenTextures(1,&id);
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1,1,0,GL_RGBA,GL_UNSIGNED_BYTE,white);
    tex = (int)id;
    return tex;
}
