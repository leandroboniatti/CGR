#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Obj3D.h"
#include "Bullet.h"
#include "ShaderProgram.h"
#include "Camera.h"

class Scene {
private:
    std::vector<std::unique_ptr<Obj3D>> objects;
    std::vector<std::unique_ptr<Bullet>> bullets;
    ShaderProgram shaderProgram;
    ShaderProgram bulletShader;
    
    // Lighting uniforms
    glm::vec3 lightPosition;
    glm::vec3 lightColor;
    
public:
    Scene();
    ~Scene();
    
    bool loadFromConfig(const std::string& configPath);
    void addObject(std::unique_ptr<Obj3D> object);
    void shoot(const glm::vec3& origin, const glm::vec3& direction);
    
    void update(float deltaTime);
    void render(const Camera& camera);
    
private:
    void updateBullets(float deltaTime);
    void checkCollisions();
    void setupLighting();
};