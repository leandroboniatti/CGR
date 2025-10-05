#include "System.h"
#include "OBJReader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

// Global pointer for callbacks
System* g_system = nullptr;

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
}

void Camera::updateVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
}

bool System::init(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    g_system = this;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* glfwWindow = glfwCreateWindow(width, height, "3D Model Viewer", nullptr, nullptr);
    if (!glfwWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    
    window = glfwWindow;
    glfwMakeContextCurrent(glfwWindow);
    
    // Set callbacks
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* w, int key, int scancode, int action, int mods) {
        if (g_system) g_system->keyCallback(w, key, scancode, action, mods);
    });
    
    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* w, double xpos, double ypos) {
        if (g_system) g_system->mouseCallback(w, xpos, ypos);
    });
    
    // Capture mouse
    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    // OpenGL configuration
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    
    // Load shaders
    if (!shader.loadFromFiles("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Failed to load shaders" << std::endl;
        return false;
    }
    
    // Create default white texture
    whiteTexture = Texture::createWhiteTexture();
    
    return true;
}

void System::shutdown() {
    if (window) {
        glfwDestroyWindow((GLFWwindow*)window);
        glfwTerminate();
    }
}

void System::update(float deltaTime) {
    GLFWwindow* glfwWindow = (GLFWwindow*)window;
    
    if (glfwWindowShouldClose(glfwWindow)) {
        return;
    }
    
    processInput(deltaTime);
    updateBullets(deltaTime);
    
    glfwPollEvents();
}

void System::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    
    // Set view and projection matrices
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix((float)windowWidth / windowHeight);
    
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("lightDir", glm::vec3(0.5f, -1.0f, 0.3f));
    
    // Render objects
    for (const auto& object : objects) {
        drawObject(object);
    }
    
    // Render bullets as small cubes (simplified)
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, bullet.position);
            model = glm::scale(model, glm::vec3(0.1f));
            shader.setMat4("model", model);
            
            // Use white texture and bind default cube if available
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);
            shader.setInt("texture_diffuse1", 0);
            
            // Draw a simple cube for bullets (you could create a simple cube mesh)
            // For now, just skip bullet rendering or implement a simple cube
        }
    }
    
    GLFWwindow* glfwWindow = (GLFWwindow*)window;
    glfwSwapBuffers(glfwWindow);
}

void System::processInput(float deltaTime) {
    GLFWwindow* glfwWindow = (GLFWwindow*)window;
    
    // Camera movement
    float velocity = camera.speed * deltaTime;
    
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.position += camera.front * velocity;
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.position -= camera.front * velocity;
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * velocity;
    }
    
    // Exit
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(glfwWindow, true);
    }
}

bool System::loadScene(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << configFile << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string objPath;
        int eliminavel;
        float tx, ty, tz, rx, ry, rz, sx, sy, sz;
        
        if (iss >> objPath >> eliminavel >> tx >> ty >> tz >> rx >> ry >> rz >> sx >> sy >> sz) {
            addObject(objPath, glm::vec3(tx, ty, tz), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz), eliminavel != 0);
        }
    }
    
    return true;
}

void System::addObject(const std::string& objPath, const glm::vec3& pos, 
                      const glm::vec3& rot, const glm::vec3& scale, bool eliminavel) {
    
    // Load mesh
    meshes.emplace_back();
    Mesh& mesh = meshes.back();
    
    if (!OBJReader::loadOBJ(objPath, mesh, materials)) {
        meshes.pop_back();
        return;
    }
    
    // Load textures for materials
    for (auto& materialPair : materials) {
        Material& material = materialPair.second;
        if (!material.mapKd.empty() && textures.find(material.mapKd) == textures.end()) {
            textures[material.mapKd] = Texture();
            if (!textures[material.mapKd].loadFromFile(material.mapKd)) {
                // Use white texture as fallback
                material.textureId = whiteTexture;
            } else {
                material.textureId = textures[material.mapKd].id;
            }
        } else if (material.mapKd.empty()) {
            material.textureId = whiteTexture;
        } else {
            material.textureId = textures[material.mapKd].id;
        }
    }
    
    // Create object
    objects.emplace_back();
    Obj3D& object = objects.back();
    object.mesh = &mesh;
    object.eliminavel = eliminavel;
    
    // Set transform
    object.transform = glm::mat4(1.0f);
    object.transform = glm::translate(object.transform, pos);
    object.transform = glm::rotate(object.transform, glm::radians(rot.x), glm::vec3(1,0,0));
    object.transform = glm::rotate(object.transform, glm::radians(rot.y), glm::vec3(0,1,0));
    object.transform = glm::rotate(object.transform, glm::radians(rot.z), glm::vec3(0,0,1));
    object.transform = glm::scale(object.transform, scale);
}

void System::fireBullet() {
    Bullet bullet;
    bullet.position = camera.position;
    bullet.direction = camera.front;
    bullet.active = true;
    bullet.timeToLive = 5.0f;
    bullet.speed = 50.0f;
    
    bullets.push_back(bullet);
}

void System::updateBullets(float deltaTime) {
    for (auto it = bullets.begin(); it != bullets.end();) {
        if (!it->active) {
            it = bullets.erase(it);
            continue;
        }
        
        // Update position
        it->position += it->direction * it->speed * deltaTime;
        it->timeToLive -= deltaTime;
        
        // Check if bullet should be removed
        if (it->timeToLive <= 0.0f) {
            it = bullets.erase(it);
            continue;
        }
        
        // Check collisions with objects
        bool hit = false;
        for (auto objIt = objects.begin(); objIt != objects.end();) {
            glm::vec3 hitPoint, hitNormal;
            if (checkBulletCollision(*it, *objIt, hitPoint, hitNormal)) {
                hit = true;
                
                if (objIt->eliminavel) {
                    // Remove both bullet and object
                    objIt = objects.erase(objIt);
                    it->active = false;
                    break;
                } else {
                    // Reflect bullet
                    it->direction = glm::reflect(it->direction, hitNormal);
                    it->position = hitPoint + hitNormal * 0.1f; // Move slightly away from surface
                    break;
                }
            } else {
                ++objIt;
            }
        }
        
        if (hit && !it->active) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

bool System::checkBulletCollision(const Bullet& bullet, Obj3D& object, glm::vec3& hitPoint, glm::vec3& hitNormal) {
    glm::vec3 boxMin = object.getWorldMin();
    glm::vec3 boxMax = object.getWorldMax();
    
    float tNear;
    bool hit = rayAABBIntersection(bullet.position, bullet.direction, boxMin, boxMax, tNear, hitNormal);
    
    if (hit) {
        hitPoint = bullet.position + bullet.direction * tNear;
    }
    
    return hit;
}

void System::drawObject(const Obj3D& object) {
    if (!object.mesh) return;
    
    shader.setMat4("model", object.transform);
    
    for (const auto& group : object.mesh->groups) {
        // Bind material texture
        auto materialIt = materials.find(group.material);
        if (materialIt != materials.end()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, materialIt->second.textureId);
            shader.setInt("texture_diffuse1", 0);
        } else {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture);
            shader.setInt("texture_diffuse1", 0);
        }
        
        glBindVertexArray(group.VAO);
        glDrawArrays(GL_TRIANGLES, 0, group.vertexCount);
    }
}

bool System::rayAABBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                const glm::vec3& boxMin, const glm::vec3& boxMax,
                                float& tNear, glm::vec3& normal) {
    glm::vec3 invDir = 1.0f / rayDir;
    glm::vec3 t1 = (boxMin - rayOrigin) * invDir;
    glm::vec3 t2 = (boxMax - rayOrigin) * invDir;
    
    glm::vec3 tMin = glm::min(t1, t2);
    glm::vec3 tMax = glm::max(t1, t2);
    
    float tNearX = tMin.x, tNearY = tMin.y, tNearZ = tMin.z;
    float tFarX = tMax.x, tFarY = tMax.y, tFarZ = tMax.z;
    
    tNear = glm::max(glm::max(tNearX, tNearY), tNearZ);
    float tFar = glm::min(glm::min(tFarX, tFarY), tFarZ);
    
    if (tNear > tFar || tFar < 0.0f) {
        return false;
    }
    
    // Calculate normal based on which face was hit
    if (tNear == tNearX) {
        normal = rayDir.x > 0 ? glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
    } else if (tNear == tNearY) {
        normal = rayDir.y > 0 ? glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
    } else {
        normal = rayDir.z > 0 ? glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
    }
    
    return true;
}

void System::keyCallback(void* window, int key, int scancode, int action, int mods) {
    (void)window; (void)scancode; (void)mods; // Suppress unused parameter warnings
    
    if (action == GLFW_PRESS) {
        g_system->keys[key] = true;
        
        if (key == GLFW_KEY_SPACE) {
            g_system->fireBullet();
        }
    } else if (action == GLFW_RELEASE) {
        g_system->keys[key] = false;
    }
}

void System::mouseCallback(void* window, double xpos, double ypos) {
    (void)window; // Suppress unused parameter warning
    
    if (g_system->firstMouse) {
        g_system->lastX = xpos;
        g_system->lastY = ypos;
        g_system->firstMouse = false;
    }
    
    float xoffset = xpos - g_system->lastX;
    float yoffset = g_system->lastY - ypos; // Reversed since y-coordinates go from bottom to top
    g_system->lastX = xpos;
    g_system->lastY = ypos;
    
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    g_system->camera.yaw += xoffset;
    g_system->camera.pitch += yoffset;
    
    // Constrain pitch
    if (g_system->camera.pitch > 89.0f) g_system->camera.pitch = 89.0f;
    if (g_system->camera.pitch < -89.0f) g_system->camera.pitch = -89.0f;
    
    g_system->camera.updateVectors();
}