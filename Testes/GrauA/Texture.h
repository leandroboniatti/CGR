#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "AssetManager.h"

class Texture {
private:
	GLuint  textureID = 0;
	GLint   textureLocation = -1; // -1 indica uniform inválido
	GLshort textureNum = 0;       // unidade de textura

public:
	Texture() = default;
	Texture(char* path, char* textureName, GLint textureUnit, GLuint shaderProgram) {
		textureNum = static_cast<GLshort>(textureUnit);
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE0 + textureNum);
		glBindTexture(GL_TEXTURE_2D, textureID);
		textureLocation = glGetUniformLocation(shaderProgram, textureName);
		if (textureLocation == -1) {
			std::cerr << "[Texture] Uniform '" << textureName << "' não encontrado.\n";
		}
		AssetManager::LoadImage(path);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if (textureLocation != -1) {
			glUniform1i(textureLocation, textureNum);
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// Destrutor padrão (não deleta textureID para permitir cópia superficial simples)
	~Texture() = default;

	// Permitir cópia superficial: todos os objetos copiarão o mesmo textureID
	// (assumindo que lifetime da GL context cobre todos os usos). Se desejar
	// ref counting no futuro, substituir por mecanismo adequado.
	Texture(const Texture&) = default;
	Texture& operator=(const Texture&) = default;

	// Move também padrão
	Texture(Texture&&) noexcept = default;
	Texture& operator=(Texture&&) noexcept = default;

	GLuint  GetTextureId()   const { return textureID; }
	GLshort GetTextureNum()  const { return textureNum; }
	GLint   GetUniformLoc()  const { return textureLocation; }
	bool    IsValid() const { return textureID != 0; }
};

#endif