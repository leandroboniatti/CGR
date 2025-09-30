#include "Shader.h"

Shader::Shader() {

	// Certifique-se de que um contexto OpenGL válido foi criado antes de executar este código.
	// Por exemplo, crie uma janela com GLFW/GLUT/SDL e chame glewInit() se necessário.

	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
	const GLchar *vertexShaderSource = R"(
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
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Deletando os shaders, eles foram linkados no programa de shader e não são mais necessários
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);	
}
	
Shader::~Shader() { }

/*
void Shader::UseTexture(std::string textureName)
{
	glActiveTexture(GL_TEXTURE0 + textures[textureName].GetTextureNum());
	glBindTexture(GL_TEXTURE_2D, textures[textureName].GetTextureId());
}

void Shader::LoadTexture(char* path, char* textureUniformName, std::string textureName)
{
	Texture tempTexture(path, textureUniformName, program, textureQtd);
	textures[textureName] = tempTexture;
	textureQtd += 1;
}
*/