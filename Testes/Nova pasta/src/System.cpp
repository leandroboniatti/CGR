#include "System.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

// Implementação da Camera
Camera::Camera() : position(0.0f, 0.0f, 3.0f), front(0.0f, 0.0f, -1.0f), 
                   up(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f), 
                   speed(5.0f), sensitivity(0.1f) {
    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

// Implementação do Projectile
Projectile::Projectile(const glm::vec3& pos, const glm::vec3& dir) 
    : position(pos), direction(glm::normalize(dir)), speed(50.0f), 
      lifetime(5.0f), active(true), VAO(0), VBO(0), buffersSetup(false) {
    setupBuffers();
}

Projectile::~Projectile() {
    cleanup();
}

void Projectile::update(float deltaTime) {
    if (!active) return;
    
    position += direction * speed * deltaTime;
    lifetime -= deltaTime;
    
    if (lifetime <= 0.0f) {
        active = false;
    }
}

bool Projectile::isExpired() const {
    return !active || lifetime <= 0.0f;
}

void Projectile::setupBuffers() {
    if (buffersSetup) return;
    
    // Criar uma pequena esfera para representar o projétil
    std::vector<float> vertices = {
        0.0f,  0.1f, 0.0f,  // top
        -0.1f, -0.1f, 0.1f,  // bottom left
         0.1f, -0.1f, 0.1f,  // bottom right
         0.1f, -0.1f, -0.1f,  // bottom back right
        -0.1f, -0.1f, -0.1f   // bottom back left
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    buffersSetup = true;
}

void Projectile::render() {
    if (!active || !buffersSetup) return;
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 15); // 5 triangles for a simple projectile
    glBindVertexArray(0);
}

void Projectile::cleanup() {
    if (buffersSetup) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        buffersSetup = false;
        VAO = VBO = 0;
    }
}

// Implementação do System
System::System() : window(nullptr), screenWidth(800), screenHeight(600), 
                   shader(nullptr), deltaTime(0.0f), lastFrame(0.0f),
                   firing(false), lastShotTime(0.0f), shotCooldown(0.3f) {
    for (int i = 0; i < 1024; ++i) {
        keys[i] = false;
    }
}

System::~System() {
    cleanup();
}

bool System::initialize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
    
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar GLFW" << std::endl;
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Criar janela
    window = glfwCreateWindow(screenWidth, screenHeight, "Visualizador de Modelos 3D", NULL, NULL);
    if (!window) {
        std::cerr << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    
    // Configurar callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Inicializar GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar GLAD" << std::endl;
        return false;
    }
    
    // Configurar OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, screenWidth, screenHeight);
    
    // Criar shader
    shader = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    
    // Configurar matriz de projeção
    projection = glm::perspective(glm::radians(45.0f), 
                                (float)screenWidth / (float)screenHeight, 
                                0.1f, 100.0f);
    
    return true;
}

void System::loadScene(const std::string& configFile) {
    std::vector<ObjectConfig> configs = parseConfig(configFile);
    
    for (const auto& config : configs) {
        Obj3D* obj = OBJReader::load(config.objFile);
        if (obj) {
            obj->setPosition(config.position);
            obj->setRotation(config.rotation);
            obj->setScale(config.scale);
            obj->eliminable = config.eliminable;
            obj->calculateCollisionRadius(); // Calcular raio de colisão
            objects.push_back(obj);
        }
    }
}

void System::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput();
        update(deltaTime);
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void System::processInput() {
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, true);
    }
    
    float velocity = camera.speed * deltaTime;
    
    // Movimento da câmera
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.position += camera.front * velocity;
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.position -= camera.front * velocity;
    }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.yaw -= camera.sensitivity * 100.0f * deltaTime;
        camera.updateVectors();
    }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.yaw += camera.sensitivity * 100.0f * deltaTime;
        camera.updateVectors();
    }
    
    // Disparo
    if (keys[GLFW_KEY_SPACE] && !firing) {
        handleShooting();
    }
    
    if (!keys[GLFW_KEY_SPACE]) {
        firing = false;
    }
}

void System::update(float deltaTime) {
    updateProjectiles(deltaTime);
    checkCollisions();
}

void System::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader->use();
    
    // Enviar matrizes para o shader
    view = camera.getViewMatrix();
    shader->setMat4("projection", glm::value_ptr(projection));
    shader->setMat4("view", glm::value_ptr(view));
    
    // Configurar iluminação
    shader->setVec3("lightPos", 10.0f, 10.0f, 10.0f);
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader->setVec3("viewPos", camera.position.x, camera.position.y, camera.position.z);
    
    // Renderizar objetos
    for (auto* obj : objects) {
        if (obj->visible) {
            obj->render(shader->ID);
        }
    }
    
    // Renderizar projéteis
    for (auto& projectile : projectiles) {
        if (projectile.active) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, projectile.position);
            shader->setMat4("model", glm::value_ptr(model));
            shader->setVec3("objectColor", 1.0f, 1.0f, 0.0f); // Amarelo para projéteis
            projectile.render();
        }
    }
}

void System::handleShooting() {
    float currentTime = glfwGetTime();
    if (currentTime - lastShotTime < shotCooldown) return;
    
    lastShotTime = currentTime;
    firing = true;
    
    // Criar projétil
    projectiles.emplace_back(camera.position, camera.front);
}

void System::updateProjectiles(float deltaTime) {
    for (auto& projectile : projectiles) {
        projectile.update(deltaTime);
    }
    
    // Remover projéteis expirados
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
                      [](const Projectile& p) { return p.isExpired(); }),
        projectiles.end());
}

void System::checkCollisions() {
    for (auto& projectile : projectiles) {
        if (!projectile.active) continue;
        
        float closestDistance = std::numeric_limits<float>::max();
        Obj3D* closestObject = nullptr;
        glm::vec3 collisionNormal;
        
        // Encontrar o objeto mais próximo que colide
        for (auto* obj : objects) {
            if (!obj->visible) continue;
            
            float distance;
            glm::vec3 normal;
            
            if (obj->intersectsWithDistance(projectile.position, projectile.direction, distance, normal)) {
                if (distance < closestDistance && distance > 0.0f) {
                    closestDistance = distance;
                    closestObject = obj;
                    collisionNormal = normal;
                }
            }
        }
        
        // Processar colisão com o objeto mais próximo
        if (closestObject) {
            if (closestObject->eliminable) {
                // Remover objeto eliminável
                auto it = std::find(objects.begin(), objects.end(), closestObject);
                if (it != objects.end()) {
                    (*it)->cleanup();
                    delete *it;
                    objects.erase(it);
                }
                projectile.active = false;
            } else {
                // Refletir projétil usando a normal calculada
                projectile.direction = reflectVector(projectile.direction, collisionNormal);
                // Mover o projétil ligeiramente para evitar colisão contínua
                projectile.position += collisionNormal * 0.1f;
            }
        }
    }
}

glm::vec3 System::reflectVector(const glm::vec3& incident, const glm::vec3& normal) {
    return incident - 2.0f * glm::dot(incident, normal) * normal;
}

void System::cleanup() {
    for (auto* obj : objects) {
        obj->cleanup();
        delete obj;
    }
    objects.clear();
    
    if (shader) {
        delete shader;
        shader = nullptr;
    }
    
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void System::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void System::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    System* system = static_cast<System*>(glfwGetWindowUserPointer(window));
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            system->keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            system->keys[key] = false;
        }
    }
}

std::vector<System::ObjectConfig> System::parseConfig(const std::string& configFile) {
    std::vector<ObjectConfig> configs;
    std::ifstream file(configFile);
    
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir arquivo de configuração: " << configFile << std::endl;
        return configs;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        ObjectConfig config;
        
        iss >> config.objFile;
        iss >> config.position.x >> config.position.y >> config.position.z;
        iss >> config.rotation.x >> config.rotation.y >> config.rotation.z;
        iss >> config.scale.x >> config.scale.y >> config.scale.z;
        iss >> config.eliminable;
        
        configs.push_back(config);
    }
    
    file.close();
    return configs;
}