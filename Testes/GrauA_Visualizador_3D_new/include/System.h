#pragma once

#include <vector>
#include <memory>
#include <string>
#include <map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "OBJ3D.h"
#include "OBJReader.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Bullet.h"

struct SceneObject {
    std::shared_ptr<OBJ3D> obj;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool eliminable;
    
    SceneObject(std::shared_ptr<OBJ3D> object, 
                const glm::vec3& pos = glm::vec3(0.0f),
                const glm::vec3& rot = glm::vec3(0.0f),
                const glm::vec3& scl = glm::vec3(1.0f),
                bool elim = true)
        : obj(object), position(pos), rotation(rot), scale(scl), eliminable(elim) {}
};

class System {
private:
    // GLFW e OpenGL
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    bool initialized;

    // Cena
    std::vector<SceneObject> sceneObjects;
    std::vector<Bullet> bullets;
    Camera camera;
    
    // Shaders e texturas
    std::shared_ptr<Shader> mainShader;
    std::map<std::string, std::shared_ptr<Texture>> textures;
    
    // Leitor de OBJ
    OBJReader objReader;
    
    // Input
    bool keys[1024];
    bool firstMouse;
    float lastX, lastY;
    
    // Timing
    float deltaTime;
    float lastFrame;

public:
    System();
    ~System();

    // Inicialização
    bool initialize(int width = 1024, int height = 768, const std::string& title = "Visualizador 3D");
    void setupOpenGL();
    void setupShaders();
    void loadScene(const std::string& configFile);

    // Loop principal
    void run();
    void update();
    void render();
    void cleanup();

    // Gerenciamento de objetos
    void addObject(const std::string& objPath, const glm::vec3& position, 
                   const glm::vec3& rotation = glm::vec3(0.0f), 
                   const glm::vec3& scale = glm::vec3(1.0f), 
                   bool eliminable = true);
    void removeObject(size_t index);
    void clearScene();

    // Sistema de tiros
    void fireBullet();
    void updateBullets();
    void checkBulletCollisions();

    // Input
    void processInput();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    // Getters
    GLFWwindow* getWindow() const;
    const Camera& getCamera() const;
    Camera& getCamera();
    bool isInitialized() const;

private:
    // Métodos auxiliares
    void updateDeltaTime();
    void setupCallbacks();
    bool loadConfigFile(const std::string& configFile);
    void positionObjectsInScene();
    bool checkObjectOverlap(const glm::vec3& position, float radius);
    glm::vec3 findValidPosition(float radius);
};