#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <glad\glad.h>
#include <stb_image.h>
//#include <SOIL.h>

#include <string>

namespace AssetManager
{

	static unsigned char* LoadImage(char* path) {
		int width, height;
		
		unsigned char* image = stbi_load(path, &width, &height, 0, 4); //SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image); //SOIL_free_image_data(image);

		return nullptr;
	};

};

#endif 