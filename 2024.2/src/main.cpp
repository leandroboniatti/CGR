#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

// Função de callback para redimensionamento da janela
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Função para processar entradas de teclado
void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Função para compilar e ligar os shaders
unsigned int compileAndLinkShaders(const char* vertexShaderSource, const char* fragmentShaderSource) {
    // Compilando Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Compilando Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Ligando os Shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Função para criar os vértices de um círculo
void createCircleVertices(float* vertices, int segments, float radius, float cx = 0.0f, float cy = 0.0f) {
    float angleIncrement = 2.0f * M_PI / segments;
    for (int i = 0; i < segments; ++i) {
        float angle = i * angleIncrement;
        vertices[3 * i] = cx + radius * cos(angle);
        vertices[3 * i + 1] = cy + radius * sin(angle);
        vertices[3 * i + 2] = 0.0f;
    }
}

int main() {
    // Inicializando GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criando a janela GLFW
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL - Desenhos Geométricos", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicializando GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Definindo os Shaders
    const char* vertexShaderSource = R"glsl(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )glsl";

    const char* fragmentShaderSource = R"glsl(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    )glsl";

    unsigned int shaderProgram = compileAndLinkShaders(vertexShaderSource, fragmentShaderSource);

    // Criando os vértices para desenhar os triângulos
    float triangleVertices[] = {
        // Primeiro Triângulo
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f,  0.5f, 0.0f,
        // Segundo Triângulo
         0.0f, -0.5f, 0.0f,
         0.9f, -0.5f, 0.0f,
         0.45f,  0.5f, 0.0f,
    };

    unsigned int VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    // Primeiro Triângulo
    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Círculo (Pac-Man)
    const int segments = 100;
    float circleVertices[segments * 3];
    createCircleVertices(circleVertices, segments, 0.5f);

    unsigned int circleVBO, circleVAO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        // Processando entradas
        processInput(window);

        // Renderizando
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Usando o programa shader
        glUseProgram(shaderProgram);

        // Desenhando os triângulos
        glBindVertexArray(VAOs[0]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Preenchido
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Contorno
        glDrawArrays(GL_TRIANGLES, 3, 3);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); // Pontos
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Desenhando o círculo (Pac-Man)
        glBindVertexArray(circleVAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLE_FAN, 0, segments);
        glBindVertexArray(0);

        // Troca dos buffers e eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpando
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteVertexArrays(1, &circleVAO);
    glDeleteBuffers(1, &circleVBO);

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
