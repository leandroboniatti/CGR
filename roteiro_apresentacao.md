# Roteiro de Apresentação - Visualizador 3D com Sistema de Colisão Avançado
**Duração: 15 minutos**

---

## **1. Introdução e Visão Geral** *(2 minutos)*

### Slide 1: Título e Objetivos
- **Projeto**: Visualizador 3D com Sistema de Colisão em C++ e OpenGL
- **Tecnologias**: C++17, OpenGL 4.6, GLFW, GLM, GLAD
- **Objetivo**: Sistema de renderização 3D com detecção de colisão precisa e projéteis físicos

### Slide 2: Funcionalidades Principais
- ✅ Carregamento de modelos OBJ com texturas
- ✅ Sistema de câmera com movimentação livre
- ✅ Projéteis com física realística
- ✅ Detecção de colisão contínua e precisa
- ✅ Reflexão de projéteis em superfícies
- ✅ Eliminação de objetos destrutíveis

---

## **2. Arquitetura do Sistema** *(3 minutos)*

### Slide 3: Estrutura de Classes
```
System (Classe Principal)
├── Camera (Controle de Visão)
├── OBJ3D (Objetos da Cena)
│   └── Mesh (Geometria e Colisão)
├── Projetil (Projéteis Físicos)
└── Shader (Renderização Unificada)
```

### Demonstração Prática:
```cpp
// Mostrar System.h - classe principal
class System {
private:
    Camera camera;
    vector<unique_ptr<OBJ3D>> sceneObjects;
    vector<unique_ptr<Projetil>> projeteis;
    Shader mainShader;
```

### Slide 4: Pipeline de Renderização
1. **Inicialização**: GLFW → OpenGL → Shaders
2. **Carregamento**: OBJ Files → Texturas → Scene Objects
3. **Loop Principal**: Input → Update → Collision → Render
4. **Cleanup**: Memory Management com Smart Pointers

---

## **3. Sistema de Colisão Avançado** *(4 minutos)*

### Slide 5: Evolução dos Algoritmos de Colisão

**checkCollisionsOld() - Problema:**
```cpp
// ❌ Detecção imediata causava "projéteis invisíveis"
if (object->rayIntersect(projetil->position, projetil->direction, distance)) {
    // Colisão detectada instantaneamente
}
```

**checkCollisions() - Solução:**
```cpp
// ✅ Detecção contínua com verificação temporal
if (object->continuousRayIntersect(rayStart, rayEnd, distance, hitPoint, hitNormal)) {
    // Colisão precisa entre duas posições
}
```

### Slide 6: Algoritmo Möller-Trumbore
```cpp
bool rayTriangleIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                         const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDirection, edge2);
    float a = glm::dot(edge1, h);
    
    // Cálculos de intersecção matemática precisa...
}
```

### Demonstração Prática:
- Mostrar arquivo `Mesh.cpp` - métodos de colisão
- Explicar diferença entre bounding box e face-level collision

---

## **4. Implementações Técnicas Destacadas** *(3 minutos)*

### Slide 7: Shader Unificado
**Antes**: 2 shaders separados (redundância)
**Depois**: 1 shader com flag condicional
```glsl
uniform bool isProjectile;

void main() {
    if (!isProjectile && hasDiffuseMap) {
        result = texture(diffuseMap, TexCoord).rgb;
    } else {
        result = objectColor; // Para projéteis
    }
}
```

### Slide 8: Sistema de Projéteis
```cpp
class Projetil {
    glm::vec3 position, direction;
    float speed, lifetime;
    
    void reflect(const glm::vec3& normal) {
        direction = glm::reflect(direction, normal);
    }
};
```

### Demonstração Prática:
- Mostrar `disparo()` method
- Explicar física de reflexão com `glm::reflect()`

---

## **5. Demonstração Interativa** *(2 minutos)*

### Execução ao Vivo:
1. **Compilar o projeto**: `g++ -std=c++17 ...`
2. **Executar**: `./visualizador3d.exe`
3. **Demonstrar funcionalidades**:
   - Movimentação da câmera (WASD + Mouse)
   - Disparo de projéteis (Espaço)
   - Colisão com objetos elimináveis
   - Reflexão em objetos não-elimináveis
   - Sistema de física em tempo real

### Casos de Teste:
- Tiro em cubo eliminável → Objeto desaparece
- Tiro em parede → Projétil reflete
- Múltiplos projéteis simultâneos
- Detecção precisa de faces

---

## **6. Conclusão e Aprendizados** *(1 minuto)*

### Slide 9: Conquistas Técnicas
- ✅ **Smart Pointers**: Gerenciamento automático de memória
- ✅ **Algoritmos Avançados**: Möller-Trumbore para precisão
- ✅ **Otimizações**: Shader unificado, métodos inline
- ✅ **Arquitetura Modular**: Fácil extensão e manutenção

### Slide 10: Possíveis Extensões
- 🔮 **Física Avançada**: Gravidade, atrito, velocidade angular
- 🔮 **Iluminação**: Phong shading, sombras dinâmicas
- 🔮 **Partículas**: Efeitos visuais de explosão
- 🔮 **Audio**: Sistema de som 3D posicional
- 🔮 **Networking**: Multiplayer com sincronização

---

## **Cronograma Detalhado:**

| Minuto | Conteúdo | Ação |
|--------|----------|------|
| 0-2 | Introdução | Apresentar projeto e objetivos |
| 2-5 | Arquitetura | Mostrar estrutura de classes |
| 5-9 | Colisão | Explicar algoritmos e evolução |
| 9-12 | Implementação | Destacar soluções técnicas |
| 12-14 | Demo | Execução ao vivo do programa |
| 14-15 | Conclusão | Resumir conquistas e futuro |

## **Dicas para Apresentação:**

1. **Preparar ambiente**: Código compilado e funcionando
2. **Backup**: Screenshots/vídeos caso haja problemas técnicos
3. **Interação**: Permitir perguntas durante demonstração
4. **Foco técnico**: Enfatizar soluções de engenharia implementadas
5. **Tempo**: Usar cronômetro para manter ritmo adequado

---

## **Comandos de Compilação:**

```bash
# Compilação do projeto
g++ -std=c++17 -g -Isrc/ -Iinclude/ -IDependencies/GLAD/include -IDependencies/GLFW/include -IDependencies/glm -IDependencies/stb_image main.cpp src/Face.cpp src/Material.cpp src/Texture.cpp src/Group.cpp src/Shader.cpp src/OBJReader.cpp src/Mesh.cpp src/OBJ3D.cpp src/Camera.cpp src/Projetil.cpp src/System.cpp Dependencies/GLAD/src/glad.c Dependencies/stb_image/stb_image.cpp -LDependencies/GLFW/lib-mingw-w64 -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32 -o visualizador3d.exe
```

## **Controles do Sistema:**

- **WASD**: Movimentação da câmera
- **Mouse**: Rotação da visão
- **Espaço**: Disparar projétil
- **ESC**: Sair da aplicação

## **Arquivos Principais a Demonstrar:**

1. `main.cpp` - Ponto de entrada
2. `include/System.h` - Classe principal
3. `src/System.cpp` - Implementação do sistema
4. `include/Mesh.h` - Sistema de colisão
5. `src/Mesh.cpp` - Algoritmos de intersecção
6. `include/Projetil.h` - Sistema de projéteis
7. `scene_config.txt` - Configuração da cena