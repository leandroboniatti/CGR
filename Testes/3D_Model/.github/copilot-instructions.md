- [x] Clarify Project Requirements
- [x] Scaffold the Project (Estrutura CMake, fontes, shaders, modelo cube.obj criada em 02/10/2025)
- [ ] Customize the Project
- [ ] Install Required Extensions
- [ ] Compile the Project (Pendente: CMake não encontrado no PATH da máquina)
- [ ] Create and Run Task
- [ ] Launch the Project
- [ ] Ensure Documentation is Complete

Resumo: Projeto C++ OpenGL 4.5 visualizador OBJ/MTL com shaders, câmera FPS e iluminação Phong.

Nota: Tentativa de execução de `cmake` falhou (não reconhecido). Necessário instalar CMake ou abrir um Developer Command Prompt/VS com CMake no PATH. Sugestões de instalação:
1. winget install Kitware.CMake
2. Ou baixar em: https://cmake.org/download/ e marcar opção "Add CMake to system PATH".
Após instalar, reexecutar etapa de configuração: cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
