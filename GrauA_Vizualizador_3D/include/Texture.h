#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

using namespace std;

class Texture {
public:
    // Carrega uma textura a partir de um arquivo e retorna o ID da textura OpenGL
    static unsigned int loadTexture(const string& path);
    // Cria uma textura a partir de dados em memória (array de bytes)
    static unsigned int createTexture(int width, int height, unsigned char* data, int channels);
    // Deleta a textura OpenGL
    static void deleteTexture(unsigned int textureID);
};

#endif