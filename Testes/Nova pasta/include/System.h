#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Obj3D.h"
#include "Shader.h"
#include "OBJReader.h"

struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    
    Camera();
    void updateVectors();
    glm::mat4 getViewMatrix();
};

struct Projectile {
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float lifetime;
    bool active;
    unsigned int VAO, VBO;
    bool buffersSetup;
    
    Projectile(const glm::vec3& pos, const glm::vec3& dir);
    ~Projectile();
    void update(float deltaTime);
    bool isExpired() const;
    void setupBuffers();
    void render();
    void cleanup();
};

class System {
public:
    System();
    ~System();
    
    bool initialize(int width = 800, int height = 600);
    void loadScene(const std::string& configFile);
    void run();
    void cleanup();
    
private:
    // GLFW window
    GLFWwindow* window;
    int screenWidth, screenHeight;
    
    // Rendering
    Shader* shader;
    glm::mat4 projection;
    glm::mat4 view;
    
    // Scene objects
    std::vector<Obj3D*> objects;
    std::vector<Projectile> projectiles;
    Camera camera;
    
    // Input
    bool keys[1024];
    float deltaTime;
    float lastFrame;
    
    // Game state
    bool firing;
    float lastShotTime;
    float shotCooldown;
    
    // Methods
    void processInput();
    void update(float deltaTime);
    void render();
    void handleShooting();
    void updateProjectiles(float deltaTime);
    void checkCollisions();
    glm::vec3 reflectVector(const glm::vec3& incident, const glm::vec3& normal);
    
    // Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    // Configuration
    struct ObjectConfig {
        std::string objFile;
        std::string mtlFile;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        bool eliminable;
    };
    
    std::vector<ObjectConfig> parseConfig(const std::string& configFile);
};