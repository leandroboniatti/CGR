#include "App.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>

static App* g_app = nullptr;

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0,0,w,h);
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = g_app; if(!app) return;
    if (app->m_firstMouse) { app->m_lastX = (float)xpos; app->m_lastY = (float)ypos; app->m_firstMouse=false; }
    float xoffset = (float)xpos - app->m_lastX;
    float yoffset = app->m_lastY - (float)ypos; // invertido
    app->m_lastX = (float)xpos; app->m_lastY = (float)ypos;
    app->m_camera.processMouse(xoffset,yoffset);
}

static void scroll_callback(GLFWwindow*, double /*xoffset*/, double yoffset) {
    if(g_app) g_app->m_camera.processScroll((float)yoffset);
}

bool App::init(int width, int height, const std::string& title) {
    m_width=width; m_height=height;
    if(!glfwInit()){ std::cerr << "Falha ao inicializar GLFW\n"; return false; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(width,height,title.c_str(), nullptr,nullptr);
    if(!m_window){ std::cerr << "Falha ao criar janela\n"; glfwTerminate(); return false; }
    glfwMakeContextCurrent(m_window);
    if(!gladLoadGL(glfwGetProcAddress)) { std::cerr << "Falha ao carregar GLAD\n"; return false; }
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(m_window, cursor_pos_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_app = this;
    glEnable(GL_DEPTH_TEST);

    std::string shaderDir = std::string(ROOT_DIR) + "/shaders";
    m_shader.load(shaderDir+"/basic.vert", shaderDir+"/basic.frag");

    // modelo inicial (cube.obj). Caso não exista, criaremos um simples substituto procedural.
    std::string modelPath = std::string(ROOT_DIR) + "/resources/models/cube.obj";
    if(!std::filesystem::exists(modelPath)) {
        std::cerr << "[App] Aviso: cube.obj não encontrado em resources/models.\n";
    } else {
        m_model.loadOBJ(modelPath);
    }
    return true;
}

void App::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(m_window, 1);
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) m_camera.processKeyboard('W', m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) m_camera.processKeyboard('S', m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) m_camera.processKeyboard('A', m_deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) m_camera.processKeyboard('D', m_deltaTime);
    static bool prevR=false, prevO=false, prevL=false;
    bool R = glfwGetKey(m_window, GLFW_KEY_R)==GLFW_PRESS;
    bool O = glfwGetKey(m_window, GLFW_KEY_O)==GLFW_PRESS;
    bool L = glfwGetKey(m_window, GLFW_KEY_L)==GLFW_PRESS;
    if (R && !prevR) { m_shader.reload(); }
    if (O && !prevO) { m_wireframe = !m_wireframe; glPolygonMode(GL_FRONT_AND_BACK, m_wireframe?GL_LINE:GL_FILL); }
    if (L && !prevL) { m_enablePointLight = !m_enablePointLight; }
    prevR=R; prevO=O; prevL=L;
}

void App::updateTitle() const {
    std::string title = "OBJ Viewer - CamPos: (" + std::to_string(m_camera.position().x) + ", " + std::to_string(m_camera.position().y) + ", " + std::to_string(m_camera.position().z) + ")";
    glfwSetWindowTitle(m_window, title.c_str());
}

void App::run() {
    while(!glfwWindowShouldClose(m_window)) {
        float current = (float)glfwGetTime();
        m_deltaTime = current - m_lastFrame;
        m_lastFrame = current;
        processInput();
        glClearColor(0.1f,0.1f,0.15f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_shader.use();
        glm::mat4 view = m_camera.viewMatrix();
        glm::mat4 proj = m_camera.projectionMatrix((float)m_width/(float)m_height);
        m_shader.setMat4("uView", view);
        m_shader.setMat4("uProj", proj);
        glm::mat4 model(1.0f);
        m_shader.setMat4("uModel", model);
        m_shader.setVec3("viewPos", m_camera.position());
        // Direcional
        m_shader.setVec3("dirLight.direction", {-0.2f,-1.0f,-0.3f});
        m_shader.setVec3("dirLight.ambient", {0.2f,0.2f,0.2f});
        m_shader.setVec3("dirLight.diffuse", {0.5f,0.5f,0.5f});
        m_shader.setVec3("dirLight.specular", {0.9f,0.9f,0.9f});
        // Ponto
        m_shader.setBool("pointLight.enabled", m_enablePointLight);
        m_shader.setVec3("pointLight.position", m_camera.position());
        m_shader.setVec3("pointLight.ambient", {0.05f,0.05f,0.05f});
        m_shader.setVec3("pointLight.diffuse", {0.8f,0.8f,0.8f});
        m_shader.setVec3("pointLight.specular", {1.0f,1.0f,1.0f});
        m_shader.setFloat("pointLight.constant", 1.0f);
        m_shader.setFloat("pointLight.linear", 0.09f);
        m_shader.setFloat("pointLight.quadratic", 0.032f);
        m_shader.setInt("diffuseTex", 0);
        m_renderer.draw(m_model.meshes(), m_shader);
        updateTitle();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void App::shutdown() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
