#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "Obj3D.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"

struct Bullet {
    glm::vec3 position;
    glm::vec3 direction;
    float speed = 50.0f;
    float timeToLive = 5.0f;
    bool active = true;
};

struct Camera {
    glm::vec3 position{0.0f, 1.5f, 5.0f};
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 60.0f;
    float speed = 5.0f;
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;
    void updateVectors();
};

class System {
public:
    bool init(int width, int height);
    void shutdown();
    void update(float deltaTime);
    void render();
    void processInput(float deltaTime);
    
    // Scene management
    bool loadScene(const std::string& configFile);
    void addObject(const std::string& objPath, const glm::vec3& pos, 
                   const glm::vec3& rot, const glm::vec3& scale, bool eliminavel);
    
    // Bullet system
    void fireBullet();
    void updateBullets(float deltaTime);
    bool checkBulletCollision(const Bullet& bullet, Obj3D& object, glm::vec3& hitPoint, glm::vec3& hitNormal);
    
    // Input callbacks
    static void keyCallback(void* window, int key, int scancode, int action, int mods);
    static void mouseCallback(void* window, double xpos, double ypos);
    
private:
    int windowWidth = 800;
    int windowHeight = 600;
    void* window = nullptr;
    
    Camera camera;
    Shader shader;
    std::vector<Obj3D> objects;
    std::vector<Mesh> meshes;
    std::unordered_map<std::string, Material> materials;
    std::unordered_map<std::string, Texture> textures;
    std::vector<Bullet> bullets;
    
    // Input state
    bool keys[1024] = {false};
    bool firstMouse = true;
    float lastX = 400.0f;
    float lastY = 300.0f;
    
    // Default white texture
    unsigned int whiteTexture = 0;
    
    void drawObject(const Obj3D& object);
    bool rayAABBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                            const glm::vec3& boxMin, const glm::vec3& boxMax,
                            float& tNear, glm::vec3& normal);
};