#include "System.h"
#include "Common.h"
#include <fstream>

// Instância global para callbacks
System* g_system = nullptr;

System::System()
    : window(nullptr)
    , windowWidth(1024)
    , windowHeight(768)
    , initialized(false)
    , firstMouse(true)
    , lastX(512.0f)
    , lastY(384.0f)
    , deltaTime(0.0f)
    , lastFrame(0.0f) {
    std::fill(keys, keys + 1024, false);
    g_system = this;
}

System::~System() {
    cleanup();
}

bool System::initialize(int width, int height, const std::string& title) {
    windowWidth = width;
    windowHeight = height;

    // Inicializa GLFW
    if (!glfwInit()) {
        std::cerr << "Erro: Falha ao inicializar GLFW" << std::endl;
        return false;
    }

    // Configura GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    // Cria janela
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Erro: Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    
    // Inicializa GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Erro: Falha ao inicializar GLAD" << std::endl;
        return false;
    }

    setupOpenGL();
    setupCallbacks();
    setupShaders();

    initialized = true;
    return true;
}

void System::setupOpenGL() {
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Configurações de entrada
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Atualiza matriz de projeção da câmera
    camera.updateProjectionMatrix((float)windowWidth / (float)windowHeight);
}

void System::setupShaders() {
    mainShader = std::make_shared<Shader>();
    
    // Shader vertex básico
    std::string vertexShader = R"(
        #version 430 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec3 aNormal;

        out vec2 TexCoord;
        out vec3 Normal;
        out vec3 FragPos;

        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;

        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            TexCoord = aTexCoord;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";

    // Shader fragment básico
    std::string fragmentShader = R"(
        #version 430 core
        out vec4 FragColor;

        in vec2 TexCoord;
        in vec3 Normal;
        in vec3 FragPos;

        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 viewPos;
        uniform vec3 objectColor;
        uniform bool hasTexture;
        uniform sampler2D texture_diffuse1;

        void main() {
            vec3 color = hasTexture ? texture(texture_diffuse1, TexCoord).rgb : objectColor;
            
            // Ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;
            
            vec3 result = (ambient + diffuse + specular) * color;
            FragColor = vec4(result, 1.0);
        }
    )";

    if (!mainShader->loadFromStrings(vertexShader, fragmentShader)) {
        std::cerr << "Erro: Falha ao compilar shaders" << std::endl;
    }
}

void System::loadScene(const std::string& configFile) {
    clearScene();
    
    // Exemplo de carregamento de cena simples
    // No arquivo de configuração, você pode especificar objetos e suas propriedades
    if (!loadConfigFile(configFile)) {
        // Se não houver arquivo de configuração, carrega objetos padrão
        std::cout << "Arquivo de configuração não encontrado. Carregando cena padrão..." << std::endl;
        
        // Adiciona alguns objetos de exemplo (se você tiver arquivos OBJ)
        // addObject("assets/cube.obj", glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(1), true);
        // addObject("assets/wall.obj", glm::vec3(-10, 0, -10), glm::vec3(0), glm::vec3(1), false);
    }
}

void System::run() {
    while (!glfwWindowShouldClose(window)) {
        updateDeltaTime();
        
        processInput();
        update();
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void System::update() {
    updateBullets();
    checkBulletCollisions();
}

void System::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mainShader->isCompiled()) return;

    mainShader->use();
    
    // Uniforms da câmera
    mainShader->setUniform("view", camera.getViewMatrix());
    mainShader->setUniform("projection", camera.getProjectionMatrix());
    mainShader->setUniform("viewPos", camera.getPosition());
    
    // Uniforms de luz
    mainShader->setUniform("lightPos", glm::vec3(10.0f, 10.0f, 10.0f));
    mainShader->setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Renderiza objetos da cena
    for (const auto& sceneObj : sceneObjects) {
        if (!sceneObj.obj->isVisible()) continue;
        
        mainShader->setUniform("model", sceneObj.obj->getTransformMatrix());
        mainShader->setUniform("objectColor", glm::vec3(0.8f, 0.6f, 0.4f));
        mainShader->setUniform("hasTexture", false);
        
        sceneObj.obj->render();
    }
    
    mainShader->unuse();
}

void System::cleanup() {
    clearScene();
    
    if (mainShader) {
        mainShader->cleanup();
    }
    
    if (window) {
        glfwDestroyWindow(window);
    }
    
    glfwTerminate();
    initialized = false;
}

void System::addObject(const std::string& objPath, const glm::vec3& position, 
                      const glm::vec3& rotation, const glm::vec3& scale, bool eliminable) {
    auto obj = objReader.loadOBJ(objPath);
    if (obj) {
        obj->setPosition(position);
        obj->setRotation(rotation);
        obj->setScale(scale);
        obj->setEliminable(eliminable);
        obj->updateTransformMatrix();
        
        sceneObjects.emplace_back(obj, position, rotation, scale, eliminable);
    }
}

void System::removeObject(size_t index) {
    if (index < sceneObjects.size()) {
        sceneObjects.erase(sceneObjects.begin() + index);
    }
}

void System::clearScene() {
    sceneObjects.clear();
    bullets.clear();
}

void System::fireBullet() {
    glm::vec3 bulletPos = camera.getPosition();
    glm::vec3 bulletDir = camera.getFront();
    
    bullets.emplace_back(bulletPos, bulletDir, 50.0f);
}

void System::updateBullets() {
    auto it = bullets.begin();
    while (it != bullets.end()) {
        it->update(deltaTime);
        
        if (!it->isActive()) {
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

void System::checkBulletCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet.isActive()) continue;
        
        for (auto it = sceneObjects.begin(); it != sceneObjects.end(); ++it) {
            if (!it->obj->isVisible()) continue;
            
            float distance;
            if (it->obj->checkRayIntersection(bullet.getPosition(), bullet.getDirection(), distance)) {
                if (distance < 1.0f) { // Distância de colisão
                    if (it->eliminable) {
                        // Remove objeto eliminável
                        it->obj->setVisible(false);
                        sceneObjects.erase(it);
                        bullet.deactivate();
                        break;
                    } else {
                        // Reflete o tiro
                        glm::vec3 normal = glm::normalize(bullet.getPosition() - it->obj->getBoundingBox().center);
                        bullet.reflect(normal);
                    }
                }
            }
        }
    }
}

void System::processInput() {
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.moveForward(deltaTime);
    }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.moveBackward(deltaTime);
    }
    if (keys[GLFW_KEY_A]) {
        camera.moveLeft(deltaTime);
    }
    if (keys[GLFW_KEY_D]) {
        camera.moveRight(deltaTime);
    }
    if (keys[GLFW_KEY_LEFT]) {
        camera.processMouseMovement(-50.0f * deltaTime, 0);
    }
    if (keys[GLFW_KEY_RIGHT]) {
        camera.processMouseMovement(50.0f * deltaTime, 0);
    }
}

void System::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!g_system) return;
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            g_system->keys[key] = true;
            
            if (key == GLFW_KEY_SPACE) {
                g_system->fireBullet();
            }
        } else if (action == GLFW_RELEASE) {
            g_system->keys[key] = false;
        }
    }
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void System::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (!g_system) return;
    
    if (g_system->firstMouse) {
        g_system->lastX = xpos;
        g_system->lastY = ypos;
        g_system->firstMouse = false;
    }
    
    float xoffset = xpos - g_system->lastX;
    float yoffset = g_system->lastY - ypos; // Y invertido
    
    g_system->lastX = xpos;
    g_system->lastY = ypos;
    
    g_system->camera.processMouseMovement(xoffset, yoffset);
}

void System::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_system) {
        g_system->camera.processMouseScroll(yoffset);
        g_system->camera.updateProjectionMatrix((float)g_system->windowWidth / (float)g_system->windowHeight);
    }
}

void System::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (g_system) {
        g_system->windowWidth = width;
        g_system->windowHeight = height;
        glViewport(0, 0, width, height);
        g_system->camera.updateProjectionMatrix((float)width / (float)height);
    }
}

GLFWwindow* System::getWindow() const {
    return window;
}

const Camera& System::getCamera() const {
    return camera;
}

Camera& System::getCamera() {
    return camera;
}

bool System::isInitialized() const {
    return initialized;
}

void System::updateDeltaTime() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void System::setupCallbacks() {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

bool System::loadConfigFile(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    // Implementação básica de carregamento de configuração
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string objPath;
        float x, y, z, rx, ry, rz, sx, sy, sz;
        bool eliminable;
        
        if (iss >> objPath >> x >> y >> z >> rx >> ry >> rz >> sx >> sy >> sz >> eliminable) {
            addObject(objPath, glm::vec3(x, y, z), glm::vec3(rx, ry, rz), glm::vec3(sx, sy, sz), eliminable);
        }
    }
    
    return true;
}

void System::positionObjectsInScene() {
    // Organiza objetos para evitar sobreposição
    for (auto& sceneObj : sceneObjects) {
        if (checkObjectOverlap(sceneObj.position, sceneObj.obj->getBoundingBox().radius)) {
            sceneObj.position = findValidPosition(sceneObj.obj->getBoundingBox().radius);
            sceneObj.obj->setPosition(sceneObj.position);
        }
    }
}

bool System::checkObjectOverlap(const glm::vec3& position, float radius) {
    for (const auto& sceneObj : sceneObjects) {
        float distance = glm::distance(position, sceneObj.position);
        if (distance < (radius + sceneObj.obj->getBoundingBox().radius)) {
            return true;
        }
    }
    return false;
}

glm::vec3 System::findValidPosition(float radius) {
    // Encontra uma posição válida sem sobreposição
    for (int attempts = 0; attempts < 100; ++attempts) {
        glm::vec3 pos(
            (rand() % 40) - 20,  // -20 a 20
            0,
            (rand() % 40) - 20   // -20 a 20
        );
        
        if (!checkObjectOverlap(pos, radius)) {
            return pos;
        }
    }
    
    return glm::vec3(0, 0, -5); // Posição padrão se não encontrar
}