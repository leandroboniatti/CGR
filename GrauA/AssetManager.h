#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <GLAD/glad.h>
#include <stb_image.h>

#include <string>
#include <iostream>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca

namespace AssetManager {
	static void LoadImage(char* path) {

		int width, height, nrChannels;
		unsigned char* image;

		image = stbi_load(path, &width, &height, &nrChannels, 0);

		if (image) {
			if (nrChannels == 3) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); }	// jpg, bmp -> 3 canais
			else { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); }				// png -> 4 canais
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else { cout << "Falha ao carregar a textura: " << path << endl; }

		stbi_image_free(image);
	}
};

#endif 