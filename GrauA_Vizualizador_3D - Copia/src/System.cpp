#include "System.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Variáveis estáticas para controle de entrada
static System* systemInstance = nullptr;
static bool shootPressed = false;

System::System() : window(nullptr), 
                   camera(glm::vec3(0.0f, 2.0f, 10.0f)),
                   deltaTime(0.0f),
                   lastFrame(0.0f),
                   firstMouse(true),
                   lastX(SCREEN_WIDTH  / 2.0f),
                   lastY(SCREEN_HEIGHT / 2.0f)
{
    systemInstance = this;

    // Inicializando o array de controle das teclas
    for (int i = 0; i < 1024; i++) { keys[i] = false; }
}

System::~System() { shutdown(); }


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

    // GLFW: Inicialização e configurações de versão do OpenGL
    glfwInit(); // Inicialização da GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // Informa a versão do OpenGL a partir da qual o código funcionará
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // Exemplo para versão 3.3 - adaptar para a versão suportada por sua placa
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); //Essencial para computadores da Apple
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);
    
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GRAU A - Ian R. Boniatti e Eduardo Tropea", NULL, NULL);

    if (!window) {
        cerr << "Falha ao criar janela GLFW" << endl;
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window); // Define a janela atual como contexto de renderização
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Ajusta a viewport quando a janela é redimensionada
    glfwSetCursorPosCallback(window, mouse_callback);   // Captura a posição do mouse
    glfwSetScrollCallback(window, scroll_callback); // Captura o scroll do mouse
    glfwSetKeyCallback(window, key_callback);   // Captura eventos do teclado
    
    // Capturar mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return true;
}

bool System::initializeOpenGL() {

    // GLAD: Inicializa e carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Falha ao inicializar GLAD" << endl;
        return false;
    }
    
    // para desenhar apenas os fragmentos mais próximos da câmera
    glEnable(GL_DEPTH_TEST);        // Ativa o teste de profundidade (z-buffer)
    glClear(GL_DEPTH_BUFFER_BIT);   // Limpa o buffer de profundidade

    // Definindo as dimensões da viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Imprimir informações do OpenGL e Placa de Vídeo
    cout << "Versao OpenGL: " << glGetString(GL_VERSION) << endl;
    cout << "Versao GLSL: "   << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "Renderer: "      << glGetString(GL_RENDERER) << endl;
    cout << "Vendor: "        << glGetString(GL_VENDOR) << endl;

    return true;
}

bool System::loadShaders() {
    // Carregar shader unificado (serve para objetos da cena e projéteis)
    string vertexShaderSource = R"(
        #version 400 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;
        layout (location = 2) in vec3 aNormal;
        
        out vec2 TexCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform bool isProjectile; // flag para diferenciar projéteis de objetos da cena
        
        void main() {
            gl_Position = projection * view * model * vec4(aPos, 1.0);
            // Só passa coordenadas de textura se não for projétil
            if (!isProjectile) {
                TexCoord = aTexCoord;
            } else {
                TexCoord = vec2(0.0, 0.0); // valor padrão para projéteis
            }
        }
    )";

    string fragmentShaderSource = R"(
        #version 400 core
        out vec4 FragColor;
        
        in vec2 TexCoord;
        
        uniform sampler2D diffuseMap;
        uniform sampler2D normalMap;
        uniform bool hasDiffuseMap;
        uniform bool hasNormalMap;
        uniform bool isProjectile; // flag para diferenciar projéteis de objetos da cena
        uniform vec3 objectColor;
        
        void main() {
            vec3 result = objectColor;
            
            // Se não for projétil e tem textura, usa a textura
            if (!isProjectile && hasDiffuseMap) {
                result = texture(diffuseMap, TexCoord).rgb;
            }
            // Caso contrário, usa a cor sólida (para projéteis ou objetos sem textura)
            
            FragColor = vec4(result, 1.0);
        }
    )";
    
    if (!mainShader.loadFromStrings(vertexShaderSource, fragmentShaderSource)) {
        return false;
    }
    
    return true;
}

// Carrega os objetos na cena
bool System::loadSceneObjects() {
    
    auto sceneObjectsInfo = readFileConfiguration(); // lê as configurações dos objetos da cena, a partir do arquivo de configuração,
                                                     // e retorna um vetor (sceneObjectsInfo) de estruturas ObjectInfo
    //vector<ObjectInfo> sceneObjectsInfo;

    for (auto& sceneObject : sceneObjectsInfo) {
        auto object = make_unique<OBJ3D>(sceneObject.name); // cria um novo objeto 3D com o nome
                                                            // especificado no arquivo de configuração

        // Tenta carregar o modelo (arquivo .obj), se falhar não adiciona o objeto à cena
        if (object->loadObject(sceneObject.modelPath)) {
            object->setPosition(sceneObject.position);
            object->setRotation(sceneObject.rotation);
            object->setScale(sceneObject.scale);
            object->setEliminable(sceneObject.eliminable);
            
            // Load texture if specified
            if (!sceneObject.texturePath.empty()) {
                object->setTexture(sceneObject.texturePath);
            }

            sceneObjects.push_back(move(object));   // adiciona o objeto 3D criado à lista de objetos da cena

            cout << "Objeto carregado: " << sceneObject.name << endl;
        } else {
            cout << "Falha ao carregar objeto: " << sceneObject.name
                 << " de " << sceneObject.modelPath << endl;
        }
    }

    cout << "Cena carregada com " << sceneObjects.size() << " objetos" << endl;
    return true;
}

// Carrega as informações/configurações dos objetos da cena
// (Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ Eliminável(S/N) TexturePath)
// a partir do arquivo de configuração da cena - "Configurador_Cena.txt"
vector<ObjectInfo> System::readFileConfiguration() {

    vector<ObjectInfo> sceneObjectsInfo;  // ObjectInfo é uma estrutura para armazenar informações sobre um determinado objeto 3D
                                          // sceneObjectsInfo é um vetor que armazena várias dessas estruturas (qtd = nº de objetos da cena)

    ifstream configFile("Configurador_Cena.txt");   // abre o arquivo de configuração para leitura

    string line;  // variável temporária para armazenar cada linha lida do arquivo de configuração

    while (getline(configFile, line)) { // loop para processar cada linha do arquivo de configuração
        if (line.empty() || line[0] == '#') continue; // Ignora linhas vazias ou comentários

        // Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ eliminável
        istringstream sline(line);  // Cria um stream a partir da linha lida
        ObjectInfo objectInfo;      // instancia estrutura para armazenar informações do objeto descrito na linha processada
        //string eliminableStr;

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
              >> objectInfo.eliminable
              >> objectInfo.texturePath;

        sceneObjectsInfo.push_back(objectInfo); // adiciona a estrutura preenchida ao vetor de configurações
    }
    
    configFile.close();
    //cout << "Carregadas " << configs.size() << " configurações de objetos" << endl;
    return sceneObjectsInfo;
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
    checkContinuousCollisions();
}

void System::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Calcula as matrizes de projeção e visualização
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 
                                          (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 
                                          0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    
    // Render scene objects
    mainShader.use();
    mainShader.setMat4("projection", projection);
    mainShader.setMat4("view", view);
    //mainShader.setVec3("viewPos", camera.Position);
    mainShader.setBool("isProjectile", false); // objetos da cena não são projéteis
    mainShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    
    for (const auto& obj : sceneObjects) { // renderiza cada objeto da cena
        obj->render(mainShader);
    }
    
    // Render projeteis usando o mesmo shader unificado
    mainShader.setBool("isProjectile", true); // agora renderizando projéteis
    mainShader.setBool("hasDiffuseMap", false); // projéteis não usam texturas
    
    for (const auto& projetil : projeteis) {
        if (projetil->isActive()) {
            projetil->draw(mainShader);
        }
    }
}

void System::handleShooting() {
    glm::vec3 projetilPos = camera.Position + camera.Front * 0.5f;  // posição inicial do projétil ligeiramente à frente da câmera
                                                                    // para evitar colisão imediata com a própria câmera
    glm::vec3 projetilDir = camera.Front; // Retorna a direção da câmera para disparo

    auto projetil = std::make_unique<Projetil>(projetilPos, projetilDir, 10.0f, 5.0f); // cria um novo projétil
    projeteis.push_back(std::move(projetil));   // adiciona o projétil à lista de projéteis ativos
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
    float yoffset = systemInstance->lastY - ypos; // coordenadas invertidas já que y-coordinates vão de baixo para cima
    
    systemInstance->lastX = xpos;
    systemInstance->lastY = ypos;
    
    systemInstance->camera.ProcessMouseMovement(xoffset, yoffset, true);
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

void System::checkContinuousCollisions() {
    for (auto& projetil : projeteis) {
        if (!projetil->isActive()) continue;
        
        glm::vec3 currentPos = projetil->getRayOrigin();
        glm::vec3 direction = projetil->getRayDirection();
        float speed = projetil->getSpeed();
        glm::vec3 nextPos = currentPos + direction * speed * deltaTime;

        // Verifica colisão com todos os objetos da cena
        // O laço for verifica se o iterador atual ainda aponta para um elemento válido.
        // Quando o iterador finalmente alcança a posição "past-the-end" do container,
        // a condição se torna falsa e o loop termina, garantindo que todos os elementos
        // válidos sejam processados sem tentar acessar memória inválida.
        for (auto object = sceneObjects.begin(); object != sceneObjects.end();) {
            float distance;
            glm::vec3 hitPoint, hitNormal;
            
            // Usar detecção contínua de colisão
            if ((*object)->continuousRayIntersect(currentPos, nextPos, distance, hitPoint, hitNormal)) {
                if ((*object)->isEliminable()) {
                    cout << "Objeto \"" << (*object)->name << "\" eliminado!" << endl;
                    object = sceneObjects.erase(object);
                    projetil->deactivate();
                } else {
                    // Usar normal precisa da face mais próxima
                    projetil->reflect(hitNormal);
                    cout << "Tiro refletiu em \"" << (*object)->name << "\"" << endl;
                    ++object;
                }
                break;
            } else {
                ++object;
            }
        }
    }
}