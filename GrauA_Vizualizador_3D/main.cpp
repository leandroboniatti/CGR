/***                             GRAU A                           ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/***  v:011025	***/
/*** Em parte adaptado do GRAU B de Fundamentos de CG e em parte do Exemplo SaberThoot ***/

// Internal

#include "src/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "src/Scene.h"
#include "src/Camera.h"
#include "src/ShaderProgram.h"

// Configurações da janela
const unsigned int WINDOW_WIDTH = 1024;
const unsigned int WINDOW_HEIGHT = 768;

// Variáveis globais
Scene* scene = nullptr;         // criado como um ponteiro nulo para o objeto Scene - acessar via scene->...
Camera* camera = nullptr;       // criado como um ponteiro nulo para o objeto Camera - acessar via camera->...
bool keys[1024];
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// Callbacks
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return -1;
    }

    // Configurar GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Visualizador 3D - CGR", nullptr, nullptr);
    if (!window) {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Capturar cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializar GLAD
    if (!gladLoadGL()) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Configurar OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Criar objetos principais
    camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    scene = new Scene();

    // Carregar cena a partir do arquivo de configuração
    if (!scene->loadFromConfig("config/scene.cfg")) {
        std::cerr << "Erro ao carregar configuração da cena" << std::endl;
        return -1;
    }

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Calcular delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        glfwPollEvents();
        processInput(window);

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Atualizar e renderizar cena
        scene->update(deltaTime);
        scene->render(*camera);

        glfwSwapBuffers(window);
    }

    // Limpeza
    delete scene;
    delete camera;
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Invertido porque y vai de baixo para cima

    lastX = xpos;
    lastY = ypos;

    camera->processMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
    // Movimento da câmera
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
        camera->processKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
        camera->processKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
        camera->processKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
        camera->processKeyboard(RIGHT, deltaTime);

    // Disparo
    if (keys[GLFW_KEY_SPACE]) {
        scene->shoot(camera->getPosition(), camera->getFront());
        keys[GLFW_KEY_SPACE] = false; // Evitar múltiplos disparos
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}