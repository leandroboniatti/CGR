#include "System.h"

System::System() { }

System::~System() { }

int System::GLFWInit() {

	// GLFW: Inicialização e configurações de versão do OpenGL	
	if (!glfwInit()) { // Inicialização da GLFW
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return EXIT_FAILURE;
    }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	// Informa a versão do OpenGL a partir da qual o código funcionará
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);	// Exemplo para versão 3.3 - Você deve adaptar para a versão do OpenGL suportada por sua placa
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);	//Essencial para computadores da Apple
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	// Criação da janela GLFW
	window = glfwCreateWindow(WIDTH, HEIGHT, "GRAU A - Ian R. Boniatti e Eduardo Tropea", nullptr, nullptr);

	if (window == nullptr) {
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	// GLAD: Inicializa e carrega todos os ponteiros de funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return EXIT_FAILURE;
	}
	
	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);

	// Obtendo as informações de versão da placa de vídeo
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); 	/* version as a string */
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	return EXIT_SUCCESS;
}

/*
int System::OpenGLSetup()
{

	glEnable(GL_BLEND);	// Enables blending ( glBlendFunc )
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	return EXIT_SUCCESS;
}
*/


/*
int System::SystemSetup() {
	coreShader = Shader("Shaders/Core/core.vert", "Shaders/Core/core.frag");
	coreShader.Use();

	return EXIT_SUCCESS;
}

void System::Run()
{

	coreShader.Use();
	coreShader.LoadTexture("images/woodTexture.jpg", "texture1", "woodTexture");

	GLfloat vertices[] =
	{
		// Positions         // Textures

		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top Right
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // Bottom Right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom Left

		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom Left
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // Top Left
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top Right
	};

	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

#pragma region Input Handling

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

#pragma endregion

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		coreShader.Use();

		coreShader.UseTexture("woodTexture");

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);


		glfwSwapBuffers(window);
	}


}
*/

void System::Finish()
{
	//coreShader.Delete();

	glfwTerminate();
}
