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
                * exemplos da Internet modificados
***/


// Internal
#include <iostream>
#include "System.h"

using namespace std;

int main() {
    //cout << "========================================" << endl;
    cout << "    Visualizador de Modelos 3D - CGR    " << endl;
    //cout << "========================================" << endl;
    cout << endl;

    System system;
    
    if (!system.initialize()) {
        cerr << "Failed to initialize system!" << endl;
        return -1;
    }
    
    system.run();
    
    return 0;
}