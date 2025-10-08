#include "System.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Variáveis estáticas para controle de entrada
static System* systemInstance = nullptr;
static bool shootPressed = false;

System::System() 
    : window(nullptr), 
      camera(glm::vec3(0.0f, 2.0f, 10.0f)),
      deltaTime(0.0f),
      lastFrame(0.0f),
      firstMouse(true),
      lastX(SCREEN_WIDTH  / 2.0f),
      lastY(SCREEN_HEIGHT / 2.0f),
      lightPos  (5.0f, 10.0f, 5.0f),
      lightColor(1.0f,  1.0f, 1.0f)
      
    {
    
    systemInstance = this;

    // Inicializando o array de controle das teclas
    for (int i = 0; i < 1024; i++) { keys[i] = false; }
}

System::~System() { shutdown(); }


bool System::initialize() {
    if (!initializeGLFW()) {
        cerr << "Falha ao inicializar GLFW" << endl;
        return false;
    }
    
    if (!initializeOpenGL()) {
        cerr << "Falha ao inicializar OpenGL" << endl;
        return false;
    }
    
    if (!loadShaders()) {
        cerr << "Falha ao carregar shaders" << endl;
        return false;
    }
    
    if (!loadSceneObjects()) {
        cerr << "Falha ao carregar objetos da cena" << endl;
        return false;
    }
    
    setupLighting();

    cout << "Sistema inicializado com sucesso" << endl;
    return true;
}

void System::run() {
    cout << "Iniciando loop principal..." << endl;
    cout << "Controles:" << endl;
    cout << "  WASD/Setas: Mover camera" << endl;
    cout << "  Mouse: Olhar ao redor" << endl;
    cout << "  ESPAÇO: Atirar" << endl;
    cout << "  ESC: Sair" << endl;
    
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput();
        update();
        render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void System::shutdown() {
    sceneObjects.clear();
    projeteis.clear();
    
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();

    cout << "Desligamento do sistema concluido" << endl;
}

bool System::initializeGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Visualizador 3D - CGR", NULL, NULL);
    if (!window) {
        cerr << "Falha ao criar janela GLFW" << endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Capturar mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return true;
}

bool System::initializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Falha ao inicializar GLAD" << endl;
        return false;
    }
    
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Imprimir informações do OpenGL
    cout << "Versao OpenGL: " << glGetString(GL_VERSION) << endl;
    cout << "Versao GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    return true;
}

bool System::loadShaders() {
    // Carregar shader principal do código incorporado
    string vertexShaderSource = R"(
        #version 400 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec3 aNormal;
        
        out vec2 TexCoord;
        out vec3 FragPos;
        out vec3 Normal;
        
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

    string fragmentShaderSource = R"(
        #version 400 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        in vec3 FragPos;
        in vec3 Normal;
        
        struct Material {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            float shininess;
            
            sampler2D diffuseMap;
            sampler2D specularMap;
            sampler2D normalMap;
            
            bool hasDiffuseMap;
            bool hasSpecularMap;
            bool hasNormalMap;
        };
        
        struct Light {
            vec3 position;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
        };
        
        uniform Material material;
        uniform Light light;
        uniform vec3 viewPos;
        uniform vec3 objectColor;
        
        void main() {
            vec3 ambient = light.ambient * material.ambient;
            if (material.hasDiffuseMap) {
                ambient *= texture(material.diffuseMap, TexCoord).rgb;
            }
            
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(light.position - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = light.diffuse * diff * material.diffuse;
            if (material.hasDiffuseMap) {
                diffuse *= texture(material.diffuseMap, TexCoord).rgb;
            }
            
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = light.specular * spec * material.specular;
            if (material.hasSpecularMap) {
                specular *= texture(material.specularMap, TexCoord).rgb;
            }
            
            vec3 result = ambient + diffuse + specular;
            FragColor = vec4(result * objectColor, 1.0);
        }
    )";
    
    if (!mainShader.loadFromStrings(vertexShaderSource, fragmentShaderSource)) {
        return false;
    }
    
    // Simple projetil shader
    string projetilVertexSource = R"(
        #version 400 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";
    
    string projetilFragmentSource = R"(
        #version 400 core
        out vec4 FragColor;
        
        uniform vec3 objectColor;
        
        void main() {
            FragColor = vec4(objectColor, 1.0);
        }
    )";
    
    if (!projetilShader.loadFromStrings(projetilVertexSource, projetilFragmentSource)) {
        return false;
    }
    
    return true;
}

bool System::loadSceneObjects() {
    auto sceneObjectsInfo = readConfiguration();
    /*
    if (sceneObjectsInfo.empty()) {
        cout << "Nenhuma configuracao encontrada, criando cena padrao..." << endl;
        
        // Create some default objects if no config is found
        sceneObjectsInfo = {
            {"Cube1", "models/cube.obj", glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), true},
            {"Cube2", "models/cube.obj", glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f), true},
            {"Wall", "models/wall.obj", glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(2.0f), false}
        };
    }
    */
    for (auto& sceneObject : sceneObjectsInfo) {
        auto object = make_unique<OBJ3D>(sceneObject.name);
        
        // Try to load the model, if it fails, continue without it
        if (object->readFile(sceneObject.modelPath)) {
            object->setPosition(sceneObject.position);
            object->setRotation(sceneObject.rotation);
            object->setScale(sceneObject.scale);
            object->setEliminable(sceneObject.eliminable);

            sceneObjects.push_back(move(object));
            cout << "Objeto carregado: " << sceneObject.name << endl;
        } else {
            cout << "Falha ao carregar objeto: " << sceneObject.name
                 << " de " << sceneObject.modelPath << endl;
        }
    }
    
    positionObjectsInScene();

    cout << "Cena carregada com " << sceneObjects.size() << " objetos" << endl;
    return true;
}

void System::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // Shooting
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !shootPressed) {
        handleShooting();
        shootPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        shootPressed = false;
    }
}

void System::update() {
    updateProjeteis();
    checkCollisions();
}

void System::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calculate matrices
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                          (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                          0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    // Render scene objects
    mainShader.use();
    mainShader.setMat4("projection", projection);
    mainShader.setMat4("view", view);
    mainShader.setVec3("viewPos", camera.Position);
    mainShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    
    // Set lighting uniforms
    mainShader.setVec3("light.position", lightPos);
    mainShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    mainShader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    mainShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    
    for (const auto& obj : sceneObjects) { // renderiza cada objeto da cena
        obj->render(mainShader);
    }
    
    // Render projeteis
    projetilShader.use();
    projetilShader.setMat4("projection", projection);
    projetilShader.setMat4("view", view);
    
    for (const auto& projetil : projeteis) {
        if (projetil->isActive()) {
            projetil->draw(projetilShader);
        }
    }
}

void System::handleShooting() {
    glm::vec3 projetilPos = camera.Position + camera.Front * 0.5f;
    glm::vec3 projetilDir = camera.GetDirection();
    
    auto projetil = std::make_unique<Projetil>(projetilPos, projetilDir, 50.0f, 10.0f);
    projeteis.push_back(std::move(projetil));
    
    //std::cout << "Tiro disparado da posição: (" << projetilPos.x << ", " 
    //          << projetilPos.y << ", " << projetilPos.z << ")" << std::endl;
}

void System::updateProjeteis() {
    for (auto& projetil : projeteis) {
        if (projetil->isActive()) {
            projetil->update(deltaTime);
        }
    }
    
    // Remove projeteis inativos
    projeteis.erase(remove_if(projeteis.begin(), projeteis.end(),
                                [](const unique_ptr<Projetil>& projetil) {
                                    return !projetil->isActive();
                                }), projeteis.end());
}

void System::checkCollisions() {
    for (auto& projetil : projeteis) {
        if (!projetil->isActive()) continue;
        
        for (auto it = sceneObjects.begin(); it != sceneObjects.end();) {
            float distance;
            if ((*it)->rayIntersect(projetil->getRayOrigin(), projetil->getRayDirection(), distance)) {
                if ((*it)->isEliminable()) {
                    cout << "Objeto \"" << (*it)->name << "\" eliminado!" << endl;
                    it = sceneObjects.erase(it);
                    projetil->deactivate();
                } else {
                    // Calculate reflection normal (simplified - using bounding box normal)
                    glm::vec3 hitPoint = projetil->getRayOrigin() + projetil->getRayDirection() * distance;
                    BoundingBox bbox = (*it)->getTransformedBoundingBox();
                    glm::vec3 center = bbox.center();
                    glm::vec3 normal = glm::normalize(hitPoint - center);
                    
                    projetil->reflect(normal);
                    cout << "Tiro refletiu em \"" << (*it)->name << "\"!" << endl;
                    ++it;
                }
                break;
            } else {
                ++it;
            }
        }
    }
}

void System::setupLighting() {
    lightPos = glm::vec3(5.0f, 10.0f, 5.0f);
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
}

// Carrega as informações/configurações dos objetos da cena
// (Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ Eliminável(S/N))
// a partir do arquivo de configuração da cena - "Configurador_Cena.txt"
vector<ObjectInfo> System::readConfiguration() {
    vector<ObjectInfo> sceneObjectsInfo;  // ObjectInfo é uma estrutura para armazenar informações sobre um determinado objeto 3D
                                          // sceneObjectsInfo é um vetor que armazena várias dessas estruturas (qtd = nº de objetos da cena)
    ifstream configFile("Configurador_Cena.txt");
    //if (!configFile.is_open()) {
    //    cout << "Arquivo de configuração da cena não encontrado (Configurador_Cena.txt)" << endl;
    //    return sceneObjectsInfo;
    //}

    string line;
    while (getline(configFile, line)) { // loop para processar cada linha do arquivo de configuração
        if (line.empty() || line[0] == '#') continue; // Ignora linhas vazias ou comentários

        // Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ eliminável
        istringstream sline(line);  // Cria um stream a partir da linha lida
        ObjectInfo objectInfo;      // instancia estrutura para armazenar informações do objeto descrito na linha processada
        string eliminableStr;

        // carrega os dados da linha para o respectivo campo da estrutura
        sline >> objectInfo.name
              >> objectInfo.modelPath
              >> objectInfo.position.x
              >> objectInfo.position.y
              >> objectInfo.position.z
              >> objectInfo.rotation.x
              >> objectInfo.rotation.y
              >> objectInfo.rotation.z
              >> objectInfo.scale.x
              >> objectInfo.scale.y
              >> objectInfo.scale.z
              >> objectInfo.eliminable;

        sceneObjectsInfo.push_back(objectInfo); // adiciona a estrutura preenchida ao vetor de configurações
    }
    
    configFile.close();
    //cout << "Carregadas " << configs.size() << " configurações de objetos" << endl;
    return sceneObjectsInfo;
}

void System::positionObjectsInScene() {
    // Ensure objects don't overlap by adjusting positions if needed
    for (size_t i = 0; i < sceneObjects.size(); i++) {
        for (size_t j = i + 1; j < sceneObjects.size(); j++) {
            auto& obj1 = sceneObjects[i];
            auto& obj2 = sceneObjects[j];
            
            glm::vec3 dist = obj1->getPosition() - obj2->getPosition();
            float distance = glm::length(dist);
            
            if (distance < 2.0f) { // Minimum distance between objects
                glm::vec3 offset = glm::normalize(dist) * (2.0f - distance) * 0.5f;
                obj1->translate(offset);
                obj2->translate(-offset);
            }
        }
    }
}

// Callback functions
void System::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void System::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!systemInstance) return;
    
    if (systemInstance->firstMouse) {
        systemInstance->lastX = xpos;
        systemInstance->lastY = ypos;
        systemInstance->firstMouse = false;
    }
    
    float xoffset = xpos - systemInstance->lastX;
    float yoffset = systemInstance->lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    systemInstance->lastX = xpos;
    systemInstance->lastY = ypos;
    
    systemInstance->camera.ProcessMouseMovement(xoffset, yoffset);
}

void System::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (systemInstance) {
        systemInstance->camera.ProcessMouseScroll(yoffset);
    }
}

void System::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (!systemInstance) return;
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            systemInstance->keys[key] = true;
        else if (action == GLFW_RELEASE)
            systemInstance->keys[key] = false;
    }
}