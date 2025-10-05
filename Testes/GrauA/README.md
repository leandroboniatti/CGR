# Visualizador de Modelos 3D

Aplicativo simples para visualização de modelos 3D no formato OBJ com funcionalidades de FPS.

## Características

- **Carregamento de modelos OBJ/MTL** com texturas
- **Triangulação automática** de faces n-gon
- **Sistema de câmera FPS** com controles WASD e mouse
- **Sistema de tiros** com colisão e reflexão
- **Objetos eliminais e indestrutíveis**
- **Bounding box** para detecção de colisão
- **Renderização com OpenGL 4.1+**

## Controles

- **W/↑**: Mover para frente
- **S/↓**: Mover para trás  
- **A/←**: Mover para esquerda
- **D/→**: Mover para direita
- **Mouse**: Olhar ao redor
- **Espaço**: Disparar tiro
- **ESC**: Sair

## Compilação

### MinGW (Windows)
```bash
# No VS Code: Ctrl+Shift+P > Tasks: Run Task > build-mingw
# Ou via terminal:
g++ -std=c++17 -O2 -Wall -Iinclude -IDependencies/GLAD/include -IDependencies/GLFW/include -IDependencies/glm -IDependencies/stb_image src/*.cpp Dependencies/GLAD/src/glad.c -LDependencies/GLFW/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -o ModelViewer.exe
```

### MSVC (Windows)
```bash
# No VS Code: Ctrl+Shift+P > Tasks: Run Task > build-msvc
# Ou no Developer Command Prompt:
cl /std:c++17 /EHsc /O2 /Iinclude /IDependencies/GLAD/include /IDependencies/GLFW/include /IDependencies/glm /IDependencies/stb_image src/*.cpp Dependencies/GLAD/src/glad.c /link opengl32.lib gdi32.lib user32.lib kernel32.lib Dependencies/GLFW/lib-vc2022/glfw3.lib /OUT:ModelViewer.exe
```

## Configuração de Cena

Edite `assets/scene.cfg` para definir objetos na cena:

```
# Formato: objPath eliminavel tx ty tz rx ry rz sx sy sz
# eliminavel: 0=indestrutível, 1=eliminável
assets/models/cube.obj 1 0 0 -5 0 0 0 1 1 1
```

## Estrutura de Classes

Seguindo o padrão UML fornecido:
- **System**: Gerenciador principal
- **OBJReader**: Carregador de arquivos OBJ/MTL
- **Mesh**: Dados geométricos
- **Obj3D**: Objeto 3D na cena
- **Group**: Grupo de faces com material
- **Face**: Face triangular
- **Material**: Propriedades do material
- **Shader**: Programa shader OpenGL
- **Texture**: Textura 2D

## Dependências

- GLAD (OpenGL loader)
- GLFW (Window/Input)
- GLM (Math)
- stb_image (Image loading)

Todas incluídas na pasta `Dependencies/`.