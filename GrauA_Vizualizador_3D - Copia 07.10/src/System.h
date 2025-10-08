#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <memory>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "OBJ3D.h"
#include "Projetil.h"

using namespace std;	// Para não precisar digitar std:: na frente de comandos da biblioteca
using namespace glm;	// Para não precisar digitar glm:: na frente de comandos da biblioteca


// Estrutura para armazenar informações sobre um determinado objeto 3D
struct ObjectConfig {
    string name;        
    string modelPath;   // Caminho para o arquivo .obj
    vec3 position;      // Posição atual do objeto na cena
    vec3 rotation;      // Rotação aplicada ao objeto
    vec3 scale;         // Escala/Dimensões do objeto
    bool eliminable;    // Se o objeto pode ser eliminado por um tiro
};

class System {
public:
    // Configurações da janela
    static const unsigned int SCREEN_WIDTH = 1024;
    static const unsigned int SCREEN_HEIGHT = 768;
    
    System();   // Construtor padrão

    ~System();  // Destrutor padrão
    
    bool initialize();
    void run();
    void shutdown();
    
private:
    GLFWwindow* window;
    Camera camera;
    Shader mainShader;
    Shader projetilShader;
    
    std::vector<std::unique_ptr<OBJ3D>> sceneObjects;
    std::vector<std::unique_ptr<Projetil>> projeteis;
    
    // Temporização
    float deltaTime;
    float lastFrame;
    
    // Entrada
    bool keys[1024];
    bool firstMouse;
    float lastX, lastY;
    
    // Iluminação
    glm::vec3 lightPos;
    glm::vec3 lightColor;
    
    bool initializeGLFW();
    bool initializeOpenGL();
    bool loadShaders();
    bool loadSceneObjects();
    
    void processInput();
    void update();
    void render();
    
    void handleShooting();
    void updateProjeteis();
    void checkCollisions();
    
    // Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    
    // Funções auxiliares
    void setupLighting();
    std::vector<ObjectConfig> loadConfiguration();
    void positionObjectsInScene();
};

#endif