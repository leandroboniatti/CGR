#pragma once
#include <string>
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Renderer.h"

struct GLFWwindow; // forward decl

class App {
public:
    bool init(int width, int height, const std::string& title);
    void run();
    void shutdown();
private:
    GLFWwindow* m_window = nullptr;
    int m_width = 1280;
    int m_height = 720;
    bool m_firstMouse = true;
    float m_lastX = m_width / 2.0f;
    float m_lastY = m_height / 2.0f;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
    bool m_wireframe = false;
    bool m_enablePointLight = true;

    Shader m_shader;
    Camera m_camera;
    Model m_model;
    Renderer m_renderer;

    void processInput();
    void updateTitle() const;
};
