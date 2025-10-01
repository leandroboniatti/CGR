#ifndef SYSTEM_H
#define SYSTEM_H

// Internal
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <assert.h>

#include <glad/glad.h> 	// biblioteca de funções baseada nas definições/especificações OPENGL - Incluir antes de outros que requerem OpenGL (como GLFW)
#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows e gerenciar entrada de teclado/mouse
#include <stb_image.h>  // biblioteca para carregar imagens (texturas)

#include <glm/glm.hpp>	// biblioteca de operações matriciais
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca

#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>

// Headers
#include "Constants.h"
#include "Shader.h"
//#include "AssetManager.h"
#include "Time.h"

class System {
	private:
		// Screen
		const GLint WIDTH = D_WIDTH, HEIGHT = D_HEIGHT;
		int screenWidth, screenHeight;

	public:
		GLFWwindow* window;
		Shader coreShader;

	public:
		System();
		~System();

		int GLFWInit();
		int ShaderSetup();
		//int OpenGLSetup();
		//void Run();
		//void Finish();
};

#endif