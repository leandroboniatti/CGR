#include "System.h"
#include <iostream>

int main() {
    std::cout << "Iniciando ModelViewer..." << std::endl;
    
    System system;
    
    // Inicializar o sistema
    std::cout << "Inicializando sistema..." << std::endl;
    if (!system.initialize(1024, 768)) {
        std::cerr << "Falha ao inicializar o sistema" << std::endl;
        std::cin.get(); // Pausa para ver erro
        return -1;
    }
    std::cout << "Sistema inicializado com sucesso!" << std::endl;
    
    // Carregar cena
    std::cout << "Carregando cena..." << std::endl;
    system.loadScene("assets/config.txt");
    std::cout << "Cena carregada!" << std::endl;
    
    std::cout << "Iniciando loop principal..." << std::endl;
    std::cout << "Controles: WASD para movimento, ESPAÇO para atirar, ESC para sair" << std::endl;
    
    // Executar loop principal
    system.run();
    
    std::cout << "Programa finalizado." << std::endl;
    return 0;
}