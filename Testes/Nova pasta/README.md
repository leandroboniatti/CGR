# Visualizador de Modelos 3D

Este é um visualizador simples de modelos 3D implementado em C++ com OpenGL 4+, usando as bibliotecas GLAD, GLFW, GLM e stb_image.

## Características

- **Carregamento de modelos OBJ**: Suporte completo para arquivos .obj com triangulação automática de faces
- **Sistema de materiais**: Suporte a texturas e propriedades de material
- **Câmera FPS**: Navegação em primeira pessoa pela cena
- **Sistema de tiro**: Mecânica de tiro com reflexão e colisão
- **Objetos elimináveis**: Alguns objetos podem ser destruídos, outros refletem projéteis
- **Detecção de colisão**: Bounding box para cada objeto

## Estrutura do Projeto

```
├── src/                 # Código fonte
├── include/             # Headers
├── shaders/             # Vertex e Fragment shaders
├── assets/              # Recursos do jogo
│   ├── models/          # Modelos 3D (.obj)
│   ├── textures/        # Texturas
│   └── config.txt       # Configuração da cena
├── Dependencies/        # Bibliotecas externas (GLAD, GLFW, GLM, stb)
└── .vscode/             # Configurações do VS Code
```

## Classes Principais

- **OBJReader**: Carregador de arquivos OBJ
- **Material**: Gerenciamento de materiais e texturas
- **Obj3D**: Representação de objetos 3D na cena
- **Group**: Grupos de faces com o mesmo material
- **Face**: Representação de faces triangulares
- **Mesh**: Container para vértices, normais e coordenadas de textura
- **System**: Sistema principal do jogo
- **Shader**: Gerenciamento de shaders
- **Texture**: Carregamento e gerenciamento de texturas

## Controles

- **WASD / Setas**: Movimentação da câmera
  - W/↑: Mover para frente
  - S/↓: Mover para trás
  - A/←: Girar para esquerda
  - D/→: Girar para direita
- **Espaço**: Atirar
- **ESC**: Sair do programa

## Compilação

### Requisitos
- Compilador C++ com suporte a C++17
- OpenGL 4.0+
- Bibliotecas GLAD, GLFW, GLM e stb_image (na pasta Dependencies)

### Usando VS Code
1. Abra o projeto no VS Code
2. Use Ctrl+Shift+P e execute "Tasks: Run Task"
3. Selecione "build" para compilar
4. Selecione "run" para executar

### Linha de comando
```bash
g++ -std=c++17 -I./include -I./Dependencies/GLAD/include -I./Dependencies/GLFW/include -I./Dependencies/GLM -I./Dependencies/stb src/*.cpp Dependencies/GLAD/src/glad.c -L./Dependencies/GLFW/lib -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -o ModelViewer.exe
```

## Configuração da Cena

Edite o arquivo `assets/config.txt` para definir os objetos da cena:

```
# Formato: arquivo_obj pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminavel
assets/models/cubo.obj 0.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1
```

- **arquivo_obj**: Caminho para o arquivo .obj
- **pos_x, pos_y, pos_z**: Posição do objeto
- **rot_x, rot_y, rot_z**: Rotação em graus
- **scale_x, scale_y, scale_z**: Escala do objeto
- **eliminavel**: 1 = pode ser destruído, 0 = indestrutível (reflete projéteis)

## Mecânica de Jogo

### Sistema de Tiro
- Projéteis são disparados na direção da câmera
- Tempo de vida limitado (5 segundos)
- Cooldown entre disparos (0.3 segundos)

### Colisões
- **Objetos elimináveis**: São removidos ao serem atingidos
- **Objetos não elimináveis**: Refletem o projétil
- Detecção baseada em bounding box (AABB)

### Câmera
- Sistema de primeira pessoa
- Movimento suave com interpolação por deltaTime
- Rotação horizontal com mouse sensitivity

## Formato OBJ Suportado

O loader suporta:
- Vértices (v)
- Coordenadas de textura (vt)
- Normais (vn)
- Faces (f) com triangulação automática
- Grupos (g)
- Comentários (#)

Exemplo de face suportada:
```
f 1/1/1 2/2/2 3/3/3  # vértice/textura/normal
```

## Shaders

### Vertex Shader
- Transformações de modelo, view e projeção
- Passagem de dados para fragment shader

### Fragment Shader
- Iluminação Phong (ambient, diffuse, specular)
- Suporte a texturas
- Cor padrão quando textura não disponível

## Estrutura de Dependencies

Certifique-se de que a pasta Dependencies contenha:
```
Dependencies/
├── GLAD/
│   ├── include/glad/glad.h
│   └── src/glad.c
├── GLFW/
│   ├── include/GLFW/glfw3.h
│   └── lib/libglfw3.a
├── GLM/
│   └── glm/
└── stb/
    └── stb_image.h
```

## Troubleshooting

### Erro de compilação
- Verifique se todas as bibliotecas estão na pasta Dependencies
- Confirme que o compilador suporta C++17
- Verifique se os caminhos dos includes estão corretos

### Erro ao executar
- Certifique-se de que os shaders estão na pasta shaders/
- Verifique se o arquivo de configuração existe
- Confirme que os modelos OBJ estão no local especificado

### Performance
- Use modelos com poucos polígonos para melhor performance
- Limite o número de objetos na cena
- Otimize texturas (tamanho e formato)

## Extensões Possíveis

- Suporte a materiais MTL
- Carregamento de texturas automático
- Iluminação dinâmica
- Sombras
- Skybox
- Física mais avançada
- Sons e efeitos visuais
- Interface gráfica (GUI)