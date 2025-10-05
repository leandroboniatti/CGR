# Visualizador de Modelos 3D - FPS Shooter

Um visualizador simples de modelos 3D em formato OBJ com funcionalidades de First Person Shooter, desenvolvido em C++ com OpenGL 4+.

## Características

- **Carregamento de modelos OBJ**: Suporte completo para arquivos .obj com materiais
- **Renderização 3D**: Shaders modernos com iluminação Phong
- **Sistema FPS**: Navegação em primeira pessoa com câmera livre
- **Sistema de tiros**: Disparo de projéteis com física de colisão e reflexão
- **Detecção de colisão**: Bounding boxes para objetos e projéteis
- **Objetos elimináveis**: Configuração de objetos que podem ser destruídos
- **Mapeamento de texturas**: Suporte para texturas difusas

## Estrutura do Projeto

```
GrauA_Visualizador_3D/
├── src/              # Código fonte
├── include/          # Headers
├── shaders/          # Shaders GLSL
├── assets/           # Modelos, texturas e configurações
├── Dependencies/     # Bibliotecas externas
└── .vscode/         # Configurações do VS Code
```

## Dependências

- **GLFW**: Gerenciamento de janelas e entrada
- **GLAD**: Carregamento de extensões OpenGL
- **GLM**: Matemática 3D
- **STB_IMAGE**: Carregamento de texturas

## Compilação

### Windows (MinGW)

```bash
# Usando VS Code Task (Ctrl+Shift+P -> "Tasks: Run Task" -> "build")
# Ou manualmente:
g++ -std=c++17 -g -O2 -I./include -I./Dependencies/GLAD/include -I./Dependencies/GLFW/include -I./Dependencies/GLM -I./Dependencies/STB_IMAGE src/*.cpp Dependencies/GLAD/src/glad.c Dependencies/STB_IMAGE/stb_image.cpp -L./Dependencies/GLFW/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -o visualizador3d.exe
```

## Controles

- **WASD / Setas**: Movimento da câmera
- **Mouse**: Rotação da câmera
- **Scroll**: Zoom
- **Espaço**: Atirar
- **ESC**: Sair

## Configuração de Cena

Edite o arquivo `assets/scene.config` para definir os objetos da cena:

```
# Formato: caminho_obj pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminavel
assets/models/cube.obj 0 0 -5 0 0 0 1 1 1 1
assets/models/wall.obj -10 0 -10 0 0 0 1 1 1 0
```

## Estrutura de Classes

- **OBJReader**: Carregamento de arquivos OBJ e MTL
- **OBJ3D**: Representação de objetos 3D com transformações
- **Material**: Propriedades de material e texturas
- **Group**: Grupos de faces para renderização otimizada
- **Face**: Representação de faces com triangulação automática
- **Shader**: Gerenciamento de shaders GLSL
- **Texture**: Carregamento e gerenciamento de texturas
- **Camera**: Câmera FPS com controles de movimento
- **Bullet**: Sistema de projéteis com física
- **System**: Classe principal do sistema

## Sistema de Colisão

- **Bounding Boxes**: Cada objeto possui uma caixa de colisão esférica
- **Ray Casting**: Detecção de intersecção raio-esfera para projéteis
- **Reflexão**: Projéteis são refletidos em objetos não-elimináveis
- **Eliminação**: Objetos marcados como elimináveis são removidos ao serem atingidos

## Recursos Implementados

### ✅ Concluído
- [x] Carregamento de arquivos OBJ
- [x] Triangulação automática de faces
- [x] Sistema de materiais e texturas
- [x] Renderização com shaders modernos
- [x] Câmera FPS com controles
- [x] Sistema de tiros e colisão
- [x] Bounding boxes para colisão
- [x] Objetos elimináveis/não-elimináveis
- [x] Reflexão de projéteis
- [x] Configuração de cena via arquivo

### 🔄 Melhorias Futuras
- [ ] Carregamento de texturas via MTL
- [ ] Normal mapping
- [ ] Sombras
- [ ] Partículas de impacto
- [ ] Sons de tiro e colisão
- [ ] Interface de usuário
- [ ] Múltiplos tipos de armas

## Exemplo de Uso

1. Compile o projeto usando o VS Code (Ctrl+Shift+P -> "Tasks: Run Task" -> "build")
2. Execute o visualizador
3. Use WASD para mover e o mouse para olhar ao redor
4. Pressione Espaço para atirar
5. Experimente atirar em diferentes objetos da cena

## Notas Técnicas

- Utiliza OpenGL 4.3 Core Profile
- Cada grupo de objeto possui seu próprio VAO para renderização otimizada
- Sistema de indexação eficiente para reduzir redundância de vértices
- Matrizes de transformação individuais para cada objeto
- Physics básico para projéteis com tempo de vida limitado

## Troubleshooting

### Erro de compilação
- Verifique se as dependências estão na pasta correta
- Confirme que o MinGW está instalado e no PATH
- Ajuste os paths no arquivo `c_cpp_properties.json` se necessário

### Objeto não carrega
- Verifique se o arquivo OBJ está no formato correto
- Confirme que os paths no arquivo de configuração estão corretos
- Verifique se há faces com menos de 3 vértices (serão ignoradas)

### Performance baixa
- Reduza o número de objetos na cena
- Use modelos com menos polígonos para teste
- Considere implementar frustum culling para cenas grandes