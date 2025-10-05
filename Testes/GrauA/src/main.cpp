#include "System.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <iostream>


int main() {
    std::cout << "Iniciando Visualizador de Modelos 3D..." << std::endl;
    System system;

    std::cout << "Carregando sistema..." << std::endl;
    if (!system.init(1280, 720)) {
        std::cerr << "Falha ao inicializar sistema" << std::endl;
        return -1;
    }
    std::cout << "Sistema inicializado com sucesso!" << std::endl;
    
    // Load scene
    std::cout << "Carregando cena..." << std::endl;
    if (!system.loadScene("assets/cena.cfg")) {
        std::cerr << "Falha ao carregar cena" << std::endl;
    } else {
        std::cout << "Cena carregada com sucesso!" << std::endl;
    }

    std::cout << "Iniciando loop principal..." << std::endl;

    auto lastTime = std::chrono::high_resolution_clock::now();
    
    // Main loop
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;
        
        system.update(deltaTime);
        system.render();
        
        // Check if window should close
        GLFWwindow* window = glfwGetCurrentContext();
        if (glfwWindowShouldClose(window)) {
            break;
        }
    }
    
    system.shutdown();
    return 0;
}