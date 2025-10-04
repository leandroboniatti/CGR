#include "Shader.h"

// Checando erros de compilação (exibição via log no terminal)
static void CheckCompile(GLuint shader, const char* label) {
    
	GLint success = 0;
	GLchar infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cerr << "ERRO::SHADER::" << shader << "::FALHA_DE_COMPILACAO\n" << infoLog << endl;
        throw runtime_error("Erro de compilação");
	}
}


// Checando por erros de linkagem
static void CheckLink(GLuint program) {

    GLint success = 0;
    GLchar infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		cerr << "ERRO::SHADER::PROGRAMA::FALHA_DE_LINKAGEM\n" << infoLog << endl;
        throw std::runtime_error("Erro de linkagem");
	}
}


// Método responsável pela compilação e montagem do programa de shader
// Os códigos fonte do vertex e do fragment shader estão nos arrays vertexShaderSource e fragmentShaderSource
// "program" (GLuint) é o identificador do programa de shader (no "main" será shaderID)
void Shader::Setup() {

    if (program != 0) return; // já inicializado


    // Verificando se o contexto OpenGL + GLAD estão ativos
    const GLubyte* openglVersion = glGetString(GL_VERSION);
    cout << "OpenGL Version: " << openglVersion << endl;
    if (!openglVersion) { throw runtime_error("[Shader] Contexto OpenGL inexistente.");}
    if (!glCreateShader || !glCreateProgram) { throw runtime_error("[Shader] Funções OpenGL (GLAD) não carregadas.");}


	// Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
    const char* vertexShaderSource = R"(
        #version 400
        layout (location = 0) in vec3 coordenadasDaGeometria;
        layout (location = 1) in vec2 coordenadasDaTextura;
        uniform mat4 projection;
        uniform mat4 model;
        out vec2 textureCoord;
        void main() {
            gl_Position = projection * model * vec4(coordenadasDaGeometria, 1.0);
            textureCoord = vec2(coordenadasDaTextura.s, 1.0 - coordenadasDaTextura.t);
        }
    )";
		// "coordenadasDaGeometria" recebe as informações que estão no local 0 -> definidas em glVertexAttribPointer(0, xxxxxxxx);
		// "coordenadasDaTextura"   recebe as informações que estão no local 1 -> definidas em glVertexAttribPointer(1, xxxxxxxx);
		// "projection" receberá as informações da forma de projeção escolhida
		// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
		// "textureCoord" enviará ao pipeline a textura de uma posição específica
		// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado


	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
    const char* fragmentShaderSource = R"(
        #version 400
        in vec2 textureCoord;
        uniform sampler2D textureBuffer;
        uniform vec2 offsetTexture;
        out vec4 color;
        void main() { color = texture(textureBuffer, textureCoord + offsetTexture);}
    )";

	// Vertex shader : Compilação
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	CheckCompile(vertexShader, "VERTEX_SHADER");

	// Fragment shader : Compilação
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	CheckCompile(fragmentShader, "FRAGMENT_SHADER");

	// Linkando os shaders e criando o identificador do programa de shader ("program")    
    program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	CheckLink(program);

    // sequência de limpeza de memória da OpenGL
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


// liberar a memória alocada pelo programa de shader
Shader::~Shader() {
    if (program) {
        glDeleteProgram(program);
        program = 0;
    }
}