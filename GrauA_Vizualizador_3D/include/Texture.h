#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

using namespace std;

class Texture {
public:
    static unsigned int loadTexture(const string& path);
    static unsigned int createTexture(int width, int height, unsigned char* data, int channels);
    static void deleteTexture(unsigned int textureID);
    
private:
    static unsigned int loadFromFile(const char* path);
};

#endif