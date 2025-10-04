#include "System.h"

#define FAILURE -1
#define SUCCESS 0


int main() {

	System system;

	if (system.GLFWInit() != 0) { return FAILURE; } 
	else { std::cout << "GLFW Initialized Successfully" << std::endl; }

	if (system.OpenGLSetup() != 0) { return FAILURE; }
	else { std::cout << "OpenGL Setup Successfully" << std::endl; }

	if (system.SystemSetup() != 0) { return FAILURE; }
	else { std::cout << "System Setup Successfully" << std::endl; }

	system.Run();

	system.Finish();

	return SUCCESS;
}