# Visualizador OBJ (OpenGL 4.5)

Projeto em C++/OpenGL para carregar e renderizar modelos em formato OBJ/MTL com texturas (map_Kd) e iluminação Phong (luz direcional + luz pontual opcional).

## Funcionalidades
- Leitura de OBJ via parser interno simples (triangulação fan)
- Suporte a múltiplos materiais / meshes
- Propriedades Ka, Kd, Ks, Ns e map_Kd
- Texturas via stb_image (flip vertical configurado)
- Shader reload (tecla `R`)
- Wireframe toggle (tecla `O`)
- Luz pontual toggle (tecla `L`)
- Câmera FPS (WASD + Mouse look, Scroll altera FOV)
- Iluminação Phong básica (direcional + ponto)
- Placeholder de textura branca automática se arquivo não encontrado

## Controles
| Tecla | Ação |
|-------|------|
| ESC | Sair |
| W A S D | Movimentação |
| Mouse | Olhar |
| Scroll | Zoom (FOV) |
| R | Recarregar shaders |
| O | Alternar wireframe |
| L | Alternar luz pontual |

## Estrutura
```
include/      # Headers (Shader, Camera, Model, etc)
src/          # Implementações
shaders/      # Shaders GLSL
resources/    # models/, materials/, textures/
```

## Dependências (FetchContent)
- GLFW
- GLAD (core 4.5)
- GLM
- stb (stb_image.h)

## Build (Windows - PowerShell)
Requisitos: CMake, Compiler (MSVC / MinGW), Git.
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel
```
Executar:
```
./build/Debug/obj_viewer.exe
```
Se usando MinGW sem configurações multi-config:
```
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/obj_viewer.exe
```

## Adicionando Modelos
Coloque arquivos `.obj` e `.mtl` em `resources/models` e `resources/materials` (ou no mesmo diretório). Ajuste o código em `App.cpp` para carregar o novo caminho.

## Próximos Passos / Melhorias
- Suporte a normal mapping / PBR
- Cache de shaders / hot reload automático via timestamps
- Suporte a múltiplas luzes
- Frustum culling e bounding volumes
- GUI (Dear ImGui) para debug

## Licenças
Verifique licenças individuais das dependências nos respectivos repositórios.
