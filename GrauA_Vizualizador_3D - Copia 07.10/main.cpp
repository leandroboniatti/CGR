/***                  GRAU A - Visualizador 3D                    ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***          Visualizador de Modelos 3D com OpenGL 4+            ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/***  v:	***/

/*** Fontes:
                * GRAU B de Fundamentos
                * Exemplo SaberThoot
                * UML dos slides "Modelagem 3D"
                * exemplos da Internet
***/


// Internal
#include <iostream>
#include "src/System.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "    Visualizador de Modelos 3D - CGR   " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    System system;
    
    if (!system.initialize()) {
        std::cerr << "Failed to initialize system!" << std::endl;
        return -1;
    }
    
    system.run();
    
    return 0;
}