#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <map>

//#include "Texture.h"


class Shader {
public:
	GLuint program;
	//std::map<std::string, Texture> textures;
	GLint textureQtd;

public:
	Shader() {

	// Certifique-se de que um contexto OpenGL válido foi criado antes de executar este código.
	
	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* vertexShaderSource = R"(
		#version 400
		layout (location = 0) in vec3 coordenadasDaGeometria;
		layout (location = 1) in vec2 coordenadasDaTextura;
		uniform mat4 projection;
		uniform mat4 model;
		out vec2 textureCoord;
		void main() {
			gl_Position = projection * model * vec4( coordenadasDaGeometria , 1.0 );
			textureCoord = vec2( coordenadasDaTextura.s , 1.0 - coordenadasDaTextura.t );
		}
	)";
		// "coordenadasDaGeometria" recebe as informações que estão no local 0 -> definidas em glVertexAttribPointer(0, xxxxxxxx);
		// "coordenadasDaTextura"   recebe as informações que estão no local 1 -> definidas em glVertexAttribPointer(1, xxxxxxxx);
		// "projection" receberá as informações da forma de projeção escolhida
		// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
		// "textureCoord" enviará ao pipeline a textura de uma posição específica
		// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado
			

	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
	const GLchar* fragmentShaderSource = R"(
		#version 400
		in vec2 textureCoord;			 // incluído
		uniform sampler2D textureBuffer; // incluído
		uniform vec2 offsetTexture;		 // incluído
		out vec4 color;
		void main() { color = texture(textureBuffer,textureCoord + offsetTexture); }	// modificado
	)";

	// Compilando os shaders
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Linkando os shaders e criando o identificador do programa de shader
	this->program = glCreateProgram();
	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, fragmentShader);
	glLinkProgram(this->program);

	// Checando por erros de linkagem
	glGetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Deletando os shaders, eles foram linkados no programa de shader e não são mais necessários
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);	
}

	void Use() { glUseProgram(this->program); }

	void Delete() { glDeleteProgram(this->program); }

//	void UseTexture(std::string textureName);

//	void LoadTexture(char* path, char* textureUniformName, std::string textureName);

	~Shader();
};

#endif