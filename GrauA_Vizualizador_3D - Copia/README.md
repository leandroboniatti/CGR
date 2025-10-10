# Visualizador de Modelos 3D - CGR

## Descrição
Aplicativo para visualização de objetos 3D em formato OBJ com renderização OpenGL 4+. Implementa um sistema de First Person Shooter com colisões e reflexões de projéteis.

## Características
- ✅ Leitura de arquivos OBJ com triangulação automática de faces
- ✅ Mapeamento de texturas
- ✅ Transformações (escala, rotação, translação)
- ✅ Sistema FPS com câmera livre
- ✅ Sistema de projéteis com colisões e reflexões
- ✅ Objetos eliminables e não eliminables
- ✅ Bounding box para detecção de colisões
- ✅ Múltiplos objetos OBJ em cena única
- ✅ Navegação com WASD/direcionais

## Estrutura de Classes
- **OBJReader**: Leitura de arquivos OBJ e MTL
- **OBJ3D**: Representação de objetos 3D na cena
- **Material**: Gerenciamento de materiais e propriedades
- **Group**: Agrupamento de faces por material
- **Face**: Representação de faces triangulares
- **Mesh**: Geometria 3D com buffers OpenGL
- **System**: Gerenciamento principal da aplicação
- **Shader**: Carregamento e gerenciamento de shaders
- **Texture**: Carregamento e gerenciamento de texturas
- **Camera**: Sistema de câmera em primeira pessoa (FPS)
- **Projetil**: Sistema de projéteis

## Controles
- **WASD** ou **Setas direcionais**: Movimento da câmera
  - ↑/W: Move para frente
  - ↓/S: Move para trás
  - ←/A: Rotaciona/move para esquerda
  - →/D: Rotaciona/move para direita
- **Mouse**: Rotação da câmera (look around)
- **Scroll**: Zoom in/out
- **Espaço**: Disparar projétil
- **ESC**: Sair da aplicação

## Compilação

### Pré-requisitos
- MinGW-w64 ou Visual Studio com C++17
- OpenGL 4.0+
- Dependências incluídas na pasta `Dependencies/`:
  - GLFW 3.x
  - GLAD
  - GLM
  - stb_image

### Usando VS Code
1. Abra o projeto no VS Code
2. Use `Ctrl+Shift+P` → "Tasks: Run Build Task" ou `Ctrl+Shift+B`
3. Selecione "Build Visualizador 3D"
4. Execute com "Run Visualizador 3D"

### Linha de comando
```bash
g++ -std=c++17 -Wall -Wextra -O2 -g ^
    -Isrc/ -IDependencies/GLAD/include -IDependencies/GLFW/include ^
    -IDependencies/glm -IDependencies/stb_image ^
    main.cpp src/*.cpp Dependencies/GLAD/src/glad.c ^
    -LDependencies/GLFW/lib-mingw-w64 ^
    -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 ^
    -o visualizador3d.exe
```

## Configuração da Cena
Edite o arquivo `Configurador_Cena.txt` para definir os objetos da cena:

```
# Formato: nome modelo posX posY posZ rotX rotY rotZ scaleX scaleY scaleZ eliminavel
Target1 models/cube.obj -3.0 0.0 -2.0 0.0 0.0 0.0 1.0 1.0 1.0 true
Wall1 models/wall.obj -5.0 0.0 -6.0 0.0 0.0 0.0 1.0 3.0 1.0 false
```

### Parâmetros:
- **nome**: Nome identificador do objeto
- **modelo**: Caminho para o arquivo .obj
- **posX/Y/Z**: Posição no espaço 3D
- **rotX/Y/Z**: Rotação em radianos (Euler angles)
- **scaleX/Y/Z**: Escala nos eixos X, Y, Z
- **eliminavel**: true = pode ser destruído, false = reflete projéteis

## Sistema de Projéteis
- Projéteis seguem a direção da câmera
- **Objetos eliminables**: Destruídos ao serem atingidos
- **Objetos não eliminables**: Refletem o projétil
- Projéteis têm tempo de vida limitado (10 segundos por padrão)
- Detecção de colisão usando bounding boxes

## Estrutura de Arquivos
```
GrauA_Vizualizador_3D/
├── main.cpp                    # Arquivo principal
├── src/                        # Código fonte
│   ├── System.h/.cpp          # Sistema principal
│   ├── Camera.h/.cpp          # Câmera FPS
│   ├── OBJ3D.h/.cpp           # Objetos 3D
│   ├── OBJReader.h/.cpp       # Leitor de arquivos OBJ
│   ├── Mesh.h/.cpp            # Geometria 3D
│   ├── Material.h/.cpp        # Materiais
│   ├── Group.h/.cpp           # Grupos de faces
│   ├── Face.h/.cpp            # Faces triangulares
│   ├── Shader.h/.cpp          # Shaders
│   ├── Texture.h/.cpp         # Texturas
│   └── Projetil.h/.cpp       # Projéteis
├── shaders/                   # Shaders GLSL
│   ├── vertex.glsl           # Vertex shader principal
│   ├── fragment.glsl         # Fragment shader principal
│   ├── projetil_vertex.glsl    # Vertex shader projéteis
│   └── projetil_fragment.glsl  # Fragment shader projéteis
├── models/                    # Modelos 3D (.obj)
├── textures/                  # Texturas (.jpg, .png)
├── Dependencies/              # Bibliotecas externas
├── .vscode/                   # Configuração VS Code
│   └── tasks.json            # Tarefas de build
├── c_cpp_properties.json     # Configuração IntelliSense
└── Configurador_Cena.txt     # Configuração da cena
```

## Recursos Implementados

### ✅ Obrigatórios
- [x] Triangulação automática de faces com 4+ vértices
- [x] Mapeamento de texturas
- [x] Transformações (escala, rotação, translação)
- [x] Sistema FPS com projéteis
- [x] Colisões com objetos eliminables/não eliminables
- [x] Reflexão de projéteis
- [x] Tempo de vida dos projéteis
- [x] Bounding box para colisões
- [x] Múltiplos objetos OBJ na cena
- [x] Navegação com câmera virtual (WASD/direcionais)

### ✅ Técnicos
- [x] Leitor próprio de arquivos OBJ
- [x] Uso de GLM para vectors e arrays
- [x] VAO por grupo de objetos
- [x] Matriz de transformação por objeto OBJ3D
- [x] Arrays de float com indexação de vértices
- [x] OpenGL 4.0+ com shaders modernos

## Exemplo de Uso
1. Coloque arquivos .obj na pasta `models/`
2. Configure `Configurador_Cena.txt` com seus objetos
3. Execute o programa
4. Use WASD para navegar
5. Espaço para atirar
6. Tente eliminar alvos e observe reflexões nos obstáculos

## Notas Técnicas
- Requer OpenGL 4.0 ou superior
- Testado com MinGW-w64 e Visual Studio
- Usa GLFW para janela e entrada
- GLAD para carregamento de funções OpenGL
- GLM para matemática 3D
- stb_image para carregamento de texturas