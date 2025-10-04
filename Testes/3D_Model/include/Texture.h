#pragma once
#include <string>

class TextureLoader {
public:
    static int loadTexture(const std::string& path, bool flipVertical = true, bool srgb = false);
    static int whiteTexture();
};
