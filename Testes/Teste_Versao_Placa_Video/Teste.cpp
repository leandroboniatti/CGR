#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

//#include <GLAD/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca

int main() {

	if (!glfwInit ()) { fprintf (stderr, "ERROR: could not start GLFW3\n");		return 1; }
	
	GLFWwindow *window = glfwCreateWindow (640, 480, "Teste de versão OpenGL", NULL, NULL);

	if (!window) { fprintf (stderr, "ERROR: could not open window with GLFW3\n");	glfwTerminate();	return 1;
	}

	glfwMakeContextCurrent (window);
	
	// inicia manipulador da extensão GLEW
	glewExperimental = GL_TRUE;
	printf ("%s\n", glewInit ());

	//if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { cout << "Failed to initialize GLAD" << std::endl; }

	// obtenção de versão suportada da OpenGL e renderizador
	const GLubyte* renderer = glGetString (GL_RENDERER);
	const GLubyte* version = glGetString (GL_VERSION);
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL (versão suportada) %s\n", version);
	// encerra contexto GL e outros recursos da GLFW
	glfwTerminate();
	return 0;
}