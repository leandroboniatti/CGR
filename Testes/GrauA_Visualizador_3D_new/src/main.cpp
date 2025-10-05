#include "Common.h"

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function 
                  << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

int main() {
    std::cout << "=== Visualizador de Modelos 3D ===" << std::endl;
    std::cout << "Controles:" << std::endl;
    std::cout << "  WASD / Setas - Movimento da câmera" << std::endl;
    std::cout << "  Mouse - Rotação da câmera" << std::endl;
    std::cout << "  Scroll - Zoom" << std::endl;
    std::cout << "  Espaço - Atirar" << std::endl;
    std::cout << "  ESC - Sair" << std::endl;
    std::cout << "=================================" << std::endl;

    System system;
    
    if (!system.initialize(1024, 768, "Visualizador 3D - FPS Shooter")) {
        std::cerr << "Erro: Falha ao inicializar o sistema" << std::endl;
        return -1;
    }

    // Carrega a cena
    system.loadScene("assets/scene.config");
    
    // Posiciona a câmera inicial
    system.getCamera().setPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    
    // Loop principal
    system.run();
    
    return 0;
}