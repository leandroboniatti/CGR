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
#include "Bullet.h"

struct ObjectConfig {
    std::string name;
    std::string modelPath;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    bool eliminable;
};

class System {
public:
    // Window settings
    static const unsigned int SCREEN_WIDTH = 1024;
    static const unsigned int SCREEN_HEIGHT = 768;
    
    System();
    ~System();
    
    bool initialize();
    void run();
    void shutdown();
    
private:
    GLFWwindow* window;
    Camera camera;
    Shader mainShader;
    Shader bulletShader;
    
    std::vector<std::unique_ptr<OBJ3D>> sceneObjects;
    std::vector<std::unique_ptr<Bullet>> bullets;
    
    // Timing
    float deltaTime;
    float lastFrame;
    
    // Input
    bool keys[1024];
    bool firstMouse;
    float lastX, lastY;
    
    // Lighting
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
    void updateBullets();
    void checkCollisions();
    
    // Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    
    // Helper functions
    void setupLighting();
    std::vector<ObjectConfig> loadConfiguration();
    void positionObjectsInScene();
};

#endif