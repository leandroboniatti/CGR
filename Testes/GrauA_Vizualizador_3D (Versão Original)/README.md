# Visualizador de Modelos 3D - CGR

## Descrição
Aplicativo para visualização de objetos 3D em formato OBJ com renderização usando OpenGL 4+ e shaders. O projeto implementa um sistema de FPS (First Person Shooter) com detecção de colisão e reflexão de projéteis.

## Características Implementadas

### ✅ Carregamento de Modelos OBJ
- Leitor próprio de arquivos OBJ
- Suporte a materiais MTL
- Carregamento de texturas
- Triangulação automática de faces com 4+ vértices

### ✅ Renderização 3D
- Shaders OpenGL 4.0+
- Mapeamento de texturas
- Iluminação Phong
- Transformações (escala, rotação, translação)

### ✅ Sistema de Câmera FPS
- Movimento com WASD/setas direcionais
- Rotação com mouse
- Controles:
  - **↑/W**: Move para frente
  - **↓/S**: Move para trás  
  - **←/A**: Rotaciona à esquerda
  - **→/D**: Rotaciona à direita
  - **ESPAÇO**: Disparar projétil

### ✅ Sistema de Projéteis
- Disparo na direção da câmera
- Detecção de colisão com bounding box
- Objetos elimináveis são removidos ao serem atingidos
- Objetos não-elimináveis refletem o projétil
- Tempo de vida limitado dos projéteis

### ✅ Gerenciamento de Cena
- Carregamento via arquivo de configuração
- Múltiplos objetos 3D na mesma cena
- Propriedades de eliminabilidade configuráveis

## Estrutura do Projeto

```
GrauA_Vizualizador_3D/
├── main.cpp                    # Arquivo principal
├── src/
│   ├── Camera.h/.cpp          # Sistema de câmera FPS
│   ├── Obj3D.h/.cpp           # Carregamento e renderização OBJ
│   ├── Scene.h/.cpp           # Gerenciamento da cena
│   ├── Bullet.h/.cpp          # Sistema de projéteis
│   ├── ShaderProgram.h/.cpp   # Gerenciamento de shaders
│   └── stb_image.h            # Carregamento de texturas
├── shaders/
│   ├── vertex.glsl            # Vertex shader principal
│   ├── fragment.glsl          # Fragment shader principal
│   ├── bullet_vertex.glsl     # Vertex shader dos projéteis
│   └── bullet_fragment.glsl   # Fragment shader dos projéteis
├── config/
│   └── scene.cfg              # Configuração da cena
├── models/                    # Diretório para modelos OBJ
├── textures/                  # Diretório para texturas
├── Makefile                   # Build sem CMake
└── README.md                  # Este arquivo
```

## Dependências

- **GLFW 3.x**: Gerenciamento de janelas e entrada
- **GLAD**: Carregamento de extensões OpenGL (incluído no projeto)
- **GLM**: Biblioteca matemática (header-only)
- **OpenGL 4.0+**: Drivers gráficos

## Compilação

### Windows (MinGW/MSYS2)
```bash
# Instalar dependências no MSYS2
pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-glm

# Compilar projeto (GLAD já incluído)
make
```

### Linux
```bash
# Ubuntu/Debian
sudo apt install libglfw3-dev libglm-dev

# Compilar projeto (GLAD já incluído)
make
```

## Configuração da Cena

Edite o arquivo `config/scene.cfg` para definir os objetos da cena:

```
# Formato: caminho_obj pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminavel(0/1)
models/cube.obj 2.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1
models/sphere.obj -2.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1
```

## Uso

1. **Compilar o projeto**:
   ```bash
   make
   ```

2. **Executar**:
   ```bash
   ./visualizador3d.exe
   ```

3. **Controles**:
   - Use mouse para olhar ao redor
   - WASD ou setas direcionais para mover
   - ESPAÇO para disparar
   - ESC para sair

## Estrutura de Dados

### Vértice
```cpp
struct Vertex {
    glm::vec3 position;  // Coordenadas 3D
    glm::vec3 normal;    // Normal do vértice
    glm::vec2 texCoords; // Coordenadas de textura
};
```

### Material
```cpp
struct Material {
    glm::vec3 ambient, diffuse, specular;
    float shininess;
    unsigned int textureID;
};
```

### Bounding Box
- Implementado como AABB (Axis-Aligned Bounding Box)
- Usado para detecção de colisão eficiente
- Calculado automaticamente a partir dos vértices

## Detecção de Colisão

- **Algoritmo**: Interseção raio-AABB
- **Projéteis**: Verificam colisão a cada frame
- **Reflexão**: Implementada usando fórmula R = D - 2(D·N)N

## Recursos Adicionais

- Triangulação automática de polígonos
- Carregamento de texturas nos formatos comuns (PNG, JPG, BMP)
- Sistema de iluminação dinâmica
- Gerenciamento automático de memória GPU

## Limitações Conhecidas

- Formato OBJ básico (sem curvas NURBS)
- Detecção de colisão limitada a bounding boxes
- Um tipo de luz (direcional)

## Desenvolvimento Futuro

- [ ] Detecção de colisão por triângulo
- [ ] Múltiplas fontes de luz
- [ ] Sombras dinâmicas
- [ ] Formato OBJ completo
- [ ] Sistema de partículas para explosões