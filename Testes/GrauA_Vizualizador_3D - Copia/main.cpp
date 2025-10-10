/***                  GRAU A - Visualizador 3D                    ***/
/*** Computação Gráfica em Tempo Real - Jogos Digitais - Unisinos ***/
/***          Visualizador de Modelos 3D com OpenGL 4+            ***/
/***        Alunos: Ian Rossetti Boniatti e Eduardo Tropea        ***/

/***  v:	***/

/*** Fontes:
                * GRAU B de Fundamentos de CG
                * Exemplo SaberThoot
                * UML dos slides "Modelagem 3D"
                * pseudos códigos dos slides de CGR
                * learnopengl.com
***/


// Internal
#include <iostream>
#include "System.h"

using namespace std;

int main() {
    cout << "    Visualizador de Modelos 3D - CGR    " << endl;
    cout << endl;

    System system;  // Instancia o sistema (janela, OpenGL, Shaders, cena, etc)

    // inicializa a GLFW (janela, contexto, callbacks, etc - na apresentação ver System.cpp)
    if (!system.initializeGLFW()) {
        cerr << "Falha ao inicializar GLFW" << endl;
        return EXIT_FAILURE; }

    // Inicializa OpenGL (GLAD, Viewport, Depth Test, etc - ver System.cpp)
    if (!system.initializeOpenGL()) {
        cerr << "Falha ao inicializar OpenGL" << endl;
        return EXIT_FAILURE; }

    // Carrega os shaders (ver System.cpp)
    if (!system.loadShaders()) {
        cerr << "Falha ao carregar shaders" << endl;
        return EXIT_FAILURE;
    }

    // Carrega os objetos da cena (ver System.cpp)
    if (!system.loadSceneObjects()) {
        cerr << "Falha ao carregar objetos da cena" << endl;
        return EXIT_FAILURE;
    }

    //system.setupBoundingBoxVisualization();

    cout << "Sistema inicializado com sucesso" << endl;

    cout << "Iniciando loop principal..." << endl;

    cout << "Controles:" << endl;
    cout << "  WASD/Setas: Mover camera" << endl;
    cout << "  Mouse: Olhar ao redor" << endl;
    cout << "  ESPAÇO: Atirar" << endl;
    cout << "  ESC: Sair" << endl;

    // Main loop - game loop
    while (!glfwWindowShouldClose(system.window)) {
        
        float currentFrame = glfwGetTime(); // Tempo atual em segundos desde que a GLFW foi inicializada 
        system.deltaTime = currentFrame - system.lastFrame; // Tempo entre frames para movimentação
        system.lastFrame = currentFrame;

        system.processInput();

        system.update();

        system.render();

        glfwSwapBuffers(system.window);

        glfwPollEvents();
    }

    system.shutdown(); // Limpa e finaliza o sistema

    return 0;
}