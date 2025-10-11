#include "System.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Variáveis estáticas para controle de entrada
static System* systemInstance = nullptr;
static bool tiroDisparado = false;

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


// Inicializa a GLFW (janela, contexto, callbacks)
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


// Inicializa OpenGL (GLAD, Viewport, Depth Test)
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


// Carrega os shaders
bool System::loadShaders() {
    // Código fonte do Vertex Shader (em GLSL - Graphics Library Shading Language)
    string vertexShaderSource = R"(
        #version 400 core
        layout (location = 0) in vec3 coordenadasDaGeometria;
        layout (location = 1) in vec2 coordenadasDaTextura;
        layout (location = 2) in vec3 coordenadasDaNormal;
        
        out vec2 textureCoord;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        uniform bool isProjectile; // flag para diferenciar projéteis de objetos da cena
        
        void main() {
            gl_Position = projection * view * model * vec4(coordenadasDaGeometria, 1.0);
            // Só passa coordenadas de textura se não for projétil
            if (!isProjectile) {
                textureCoord = coordenadasDaTextura;
            } else {
                textureCoord = vec2(0.0, 0.0); // valor padrão para projéteis
            } 
        }
    )";
   		// "coordenadasDaGeometria" recebe as informações que estão no local 0 -> definidas em glVertexAttribPointer(0, xxxxxxxx);
		// "coordenadasDaTextura"   recebe as informações que estão no local 1 -> definidas em glVertexAttribPointer(1, xxxxxxxx);
        // "coordenadasDaNormal"    recebe as informações que estão no local 2 -> definidas em glVertexAttribPointer(2, xxxxxxxx);
		// "model" receberá as informações das transformações a serem aplicadas (translação, escala, rotação)
        // "view" receberá as informações da câmera (posição, direção, etc.)
        // "projection" receberá as informações da forma de projeção escolhida
		// "textureCoord" enviará ao pipeline a textura de uma posição específica
		// "gl_Position" é uma variável específica do GLSL que recebe a posição final do vertice processado


	//Código fonte do Fragment Shader (em GLSL - Graphics Library Shading Language)
    string fragmentShaderSource = R"(
        #version 400 core
        out vec4 FragColor;
        
        in vec2 textureCoord;
        
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
                result = texture(diffuseMap, textureCoord).rgb;
            }
            // Caso contrário, usa a cor sólida (para projéteis ou objetos sem textura)
            
            FragColor = vec4(result, 1.0);
        }
    )";
    
    // Compila e linka os shaders
    if (!mainShader.loadFromStrings(vertexShaderSource, fragmentShaderSource)) {
        return false;
    }
    
    return true;
}


// Carrega os objetos na cena
bool System::loadSceneObjects() {
                                                     // na apresentação: ver readFileConfiguration() logo abaixo
    auto sceneObjectsInfo = readFileConfiguration(); // lê as configurações dos objetos da cena, a partir do arquivo de configuração,
                                                     // e retorna um vetor (sceneObjectsInfo) de estruturas ObjectInfo

    for (auto& sceneObject : sceneObjectsInfo) { // loop para processar cada configuração de objeto lida do arquivo
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
        }
        else {
            cout << "Falha ao carregar objeto: " << sceneObject.name
                 << " de " << sceneObject.modelPath << endl;
        }
    }

    return true;
}


// Carrega as informações/configurações dos objetos da cena a partir do arquivo de configuração da cena - "Configurador_Cena.txt"
// (Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ Eliminável(S/N) TexturePath)
vector<ObjectInfo> System::readFileConfiguration() {

    vector<ObjectInfo> sceneObjectsInfo;  // ObjectInfo é uma estrutura para armazenar informações sobre um determinado objeto 3D
                                          // sceneObjectsInfo é um vetor que armazena várias dessas estruturas (qtd = nº de objetos da cena)

    ifstream configFile("Configurador_Cena.txt");   // abre o arquivo de configuração para leitura

    string line;  // variável temporária para armazenar cada linha lida do arquivo de configuração

    while (getline(configFile, line)) { // loop para processar cada linha do arquivo de configuração
        if (line.empty() || line[0] == '#') continue; // Ignora linhas vazias ou comentários

        // Nome Path posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ eliminável
        istringstream sline(line);  // Cria um stream (sline) a partir da linha lida
        ObjectInfo objectInfo;      // instancia estrutura para armazenar informações do objeto descrito na linha processada
        //string eliminableStr;

        // carrega os dados da linha para o respectivo campo da estrutura objectInfo
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
    }                                           // vetor de estruturas ObjectInfo
    
    configFile.close();

    return sceneObjectsInfo;
}


// Processa a entrada do usuário
void System::processInput() {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // movimentação da câmera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    // Disparo
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !tiroDisparado) {
        disparo();
        tiroDisparado = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        tiroDisparado = false;
    }
}


// Renderiza a cena
void System::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calcula a matriz de projeção - glm::perspective(FOV, razão de aspecto, Near, Far) - razão de aspecto = largura/altura
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Calcula a matriz de visualização - glm::lookAt(posição da câmera, ponto para onde a câmera está olhando, vetor up da câmera)
    glm::mat4 view = camera.GetViewMatrix(); // glm::lookAt(Position, Position + Front, Up)
    
    // renderiza objetos da cena
    mainShader.use();
    mainShader.setMat4("projection", projection);
    mainShader.setMat4("view", view);
    //mainShader.setVec3("viewPos", camera.Position);
    mainShader.setBool("isProjectile", false); // objetos da cena não são projéteis
    mainShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
    
    for (const auto& obj : sceneObjects) { // renderiza cada objeto da cena
        obj->render(mainShader);
    }
    
    // Render projeteis
    mainShader.setBool("isProjectile", true); // agora renderizando projéteis
    mainShader.setBool("hasDiffuseMap", false); // projéteis não usam texturas
    
    for (const auto& projetil : projeteis) {
        if (projetil->isActive()) {
            projetil->draw(mainShader);
        }
    }
}


// realiza o disparo de um projétil a partir da posição e direção da câmera
void System::disparo() {
    glm::vec3 projetilPos = camera.Position + camera.Front * 0.5f;  // posição inicial do projétil ligeiramente à frente da câmera
                                                                    // para evitar colisão imediata com a própria câmera
    glm::vec3 projetilDir = camera.Front; // Retorna a direção da câmera para disparo

    auto projetil = std::make_unique<Projetil>(projetilPos, projetilDir, 10.0f, 5.0f); // cria um novo projétil
    projeteis.push_back(std::move(projetil));   // adiciona o projétil à lista de projéteis ativos
}


// Atualiza a posição dos projéteis e remove os inativos
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


// funções de callback estáticas
void System::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


// mouse_callback para movimentação da câmera
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


// scroll_callback para zoom da câmera
void System::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (systemInstance) {
        systemInstance->camera.ProcessMouseScroll(yoffset);
    }
}


// key_callback para controle de teclas
void System::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (!systemInstance) return;
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            systemInstance->keys[key] = true;
        else if (action == GLFW_RELEASE)
            systemInstance->keys[key] = false;
    }
}


// Verifica colisões entre projéteis e objetos da cena - tem problema na reflexão!
void System::checkCollisions() {
    const float MIN_DISTANCE = 0.1f; // Distância mínima segura antes de verificar colisões

    for (auto& projetil : projeteis) {
        if (!projetil->isActive()) continue;
        
        // Só verifica colisões se o projétil já percorreu distância mínima
        if (projetil->lifetime < MIN_DISTANCE / projetil->speed) {
            continue;
        }

        for (auto sceneObject = sceneObjects.begin(); sceneObject != sceneObjects.end();) {
            float distance;
            
            // Calcular próxima posição do projétil para verificação de colisão
            glm::vec3 nextPosition = projetil->position + projetil->direction * projetil->speed * deltaTime;
            
            if ((*sceneObject)->rayIntersect(projetil->position, projetil->direction, distance)) {
                // Verificar se a colisão acontecerá no próximo frame (não imediatamente)
                if (distance <= projetil->speed * deltaTime * 1.1f && distance > 0.0f) {
                    if ((*sceneObject)->isEliminable()) {
                        cout << "Objeto \"" << (*sceneObject)->name << "\" eliminado!" << endl;
                        sceneObject = sceneObjects.erase(sceneObject);
                        projetil->desativar();
                    } else {
                        // Calcular ponto de impacto mais preciso
                        glm::vec3 hitPoint = projetil->position + projetil->direction * distance;
                        BoundingBox bbox = (*sceneObject)->getTransformedBoundingBox();
                        glm::vec3 center = bbox.center();
                        glm::vec3 normal = glm::normalize(hitPoint - center);
                        
                        // Mover projétil para posição de colisão antes de refletir
                        projetil->position = hitPoint + normal * 0.01f; // Pequeno offset para evitar re-colisão
                        projetil->reflect(normal);
                        cout << "Tiro refletiu em \"" << (*sceneObject)->name << "\"!" << endl;
                        ++sceneObject;
                    }
                    break;
                } else {
                    ++sceneObject; // Colisão muito distante, continua verificando
                }
            } else {
                ++sceneObject; // Nenhuma colisão, verifica o próximo objeto
            }
        }
    }
}