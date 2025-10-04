#ifndef SYSTEM_H
#define SYSTEM_H

// External
#include <glad/glad.h>  // biblioteca de funções baseada nas definições/especificações OPENGL - Incluir antes de outros que requerem OpenGL (como GLFW)
#include <GLFW/glfw3.h> // biblioteca de funções para criação da janela no Windows e gerenciar entrada de teclado/mouse
#include <stb_image.h>

// GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Headers
//#include "Shader.h"
//#include "AssetManager.h"
//#include "Time.h"

// Internal
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <math.h>



class System {
	private:
		// Screen
		const GLint WIDTH = 600, HEIGHT = 600;
		int screenWidth, screenHeight;

	public:
		GLFWwindow* window;
		//Shader coreShader;

	public:
		System();
		~System();

		int GLFWInit();
		int OpenGLSetup();
		int SystemSetup();

		void Run();
		void Finish();
};

#endif