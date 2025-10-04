#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca

class Shader {
	public:
		GLuint program = 0;
		//GLint textureQtd = 0;

	public:
		// Construtor padrão. Chamamos Initialize() depois de criar o contexto OPENGL + GLAD.
		Shader() = default;

		// Faz toda a compilação/link dos shaders
		void Setup();

		void Use() {
			if (program == 0) {
				cerr << "[Shader] Programa inválido (program == 0)\n";
				return;
			}
			glUseProgram(program);
		}

		void Delete() {
			if (program) {
				glDeleteProgram(program);
				program = 0;
			}
		}

		~Shader();
	};
#endif