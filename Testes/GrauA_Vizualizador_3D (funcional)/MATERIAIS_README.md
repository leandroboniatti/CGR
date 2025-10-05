# Sistema de Materiais - Visualizador 3D

## Visão Geral

O sistema de materiais foi aprimorado significativamente para suportar renderização realística com múltiplas propriedades de material e texturas. O sistema agora inclui:

### Funcionalidades Implementadas

#### 1. **Estrutura de Material Expandida**
- **Propriedades básicas**: Ambient, Diffuse, Specular, Shininess
- **Propriedades avançadas**: Emission, Transparency, Optical Density
- **Suporte a texturas**: Diffuse, Specular, Normal mapping
- **Modelo de iluminação**: Suporte a diferentes modelos (Blinn-Phong, etc.)

#### 2. **Sistema de Cache de Texturas**
- **Carregamento inteligente**: Evita carregar a mesma textura múltiplas vezes
- **Gerenciamento de memória**: Cache compartilhado entre objetos
- **Tratamento de erros**: Fallback para texturas padrão em caso de erro

#### 3. **Materiais Procedurais**
Métodos utilitários para criar materiais pré-definidos:
- `createMetallicMaterial()` - Material metálico realístico
- `createPlasticMaterial()` - Material plástico brilhante
- `createEmissiveMaterial()` - Material emissor de luz

#### 4. **Shaders Melhorados**
- **Fragment Shader**: Suporte a múltiplas texturas e emission
- **Normal Mapping**: Implementação básica para detalhes de superfície
- **Flags condicionais**: Ativação de texturas apenas quando disponíveis

## Materiais Criados

### 1. **Cubo Metálico** (`MetallicCube`)
```mtl
Ka 0.1 0.1 0.15     # Ambient azulado escuro
Kd 0.3 0.3 0.4      # Diffuse metálico
Ks 0.9 0.9 0.95     # Specular muito alto
Ns 128.0            # Shininess alto
```

### 2. **Esfera Plástica** (`SpherePlastic`)
```mtl
Ka 0.05 0.05 0.2    # Ambient azul escuro
Kd 0.3 0.5 0.9      # Diffuse azul vivo
Ks 0.8 0.8 0.9      # Specular alto
Ns 96.0             # Shininess moderado-alto
```

### 3. **Chão Grama** (`GroundGrass`)
```mtl
Ka 0.1 0.2 0.1      # Ambient verde escuro
Kd 0.3 0.7 0.3      # Diffuse verde natural
Ks 0.2 0.3 0.2      # Specular baixo (natural)
Ns 4.0              # Shininess muito baixo
```

### 4. **Pirâmide Dourada** (`PyramidGold`)
```mtl
Ka 0.24725 0.1995 0.0745    # Ambient dourado
Kd 0.75164 0.60648 0.22648  # Diffuse dourado
Ks 0.628281 0.555802 0.366065 # Specular dourado
Ns 51.2                     # Shininess médio
```

### 5. **Parede Tijolo** (`WallBrick`)
```mtl
Ka 0.2 0.1 0.08     # Ambient vermelho escuro
Kd 0.7 0.3 0.2      # Diffuse tijolo
Ks 0.3 0.2 0.2      # Specular baixo
Ns 12.0             # Shininess baixo
```

## API de Materiais

### Métodos Principais

#### Gerenciamento de Materiais
```cpp
// Adicionar material customizado
void addMaterial(const Material& material);

// Atualizar material existente
bool updateMaterial(const std::string& name, const Material& newMaterial);

// Definir material padrão
void setDefaultMaterial();

// Obter lista de materiais
const std::vector<Material>& getMaterials() const;
```

#### Criação de Materiais Procedurais
```cpp
// Material metálico
Material metal = Obj3D::createMetallicMaterial(
    glm::vec3(0.7f, 0.7f, 0.8f),  // cor
    0.1f                           // roughness
);

// Material plástico
Material plastic = Obj3D::createPlasticMaterial(
    glm::vec3(1.0f, 0.2f, 0.2f),  // cor
    64.0f                          // shininess
);

// Material emissor
Material emissive = Obj3D::createEmissiveMaterial(
    glm::vec3(1.0f, 1.0f, 0.0f),  // cor
    2.0f                           // intensidade
);
```

### Cache de Texturas
```cpp
// Limpar cache (liberar memória)
void clearTextureCache();

// As texturas são automaticamente cacheadas durante o carregamento
```

## Estrutura de Arquivos

### Arquivos MTL
Localização: `objetos/*.mtl`
- `cube.mtl` - Material metálico
- `sphere.mtl` - Material plástico azul
- `ground.mtl` - Material grama verde
- `pyramid.mtl` - Material dourado
- `wall.mtl` - Material tijolo vermelho

### Arquivos de Shader
- `shaders/vertex.glsl` - Vertex shader básico
- `shaders/fragment.glsl` - Fragment shader com suporte a materiais avançados

## Propriedades Suportadas

### Propriedades MTL Suportadas
- `Ka` - Cor ambiente (Ambient)
- `Kd` - Cor difusa (Diffuse)
- `Ks` - Cor especular (Specular)
- `Ke` - Cor emissiva (Emission)
- `Ns` - Expoente especular (Shininess)
- `d`/`Tr` - Transparência
- `Ni` - Densidade óptica
- `illum` - Modelo de iluminação
- `map_Kd` - Textura difusa
- `map_Ks` - Textura especular
- `map_Bump`/`bump` - Mapa de normais

### Uniformes do Shader
```glsl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

uniform Material material;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform bool hasTexture;
uniform bool hasSpecularTexture;
uniform bool hasNormalTexture;
```

## Iluminação

### Modelo de Iluminação
O sistema utiliza o modelo **Blinn-Phong** com as seguintes componentes:
- **Ambient**: Iluminação ambiente constante
- **Diffuse**: Iluminação difusa baseada na direção da luz
- **Specular**: Reflexão especular baseada na posição da câmera
- **Emission**: Emissão própria do material

### Controles de Luz
- Posição da luz: `lightPos` (vec3)
- Cor da luz: `lightColor` (vec3)
- Posição da câmera: `viewPos` (vec3)

## Performance

### Otimizações Implementadas
1. **Cache de texturas**: Evita recarregamento
2. **Material padrão**: Fallback eficiente
3. **Flags condicionais**: Apenas processa texturas disponíveis
4. **Mapeamento direto**: Busca eficiente de materiais por nome

## Uso

### Configuração na Cena
No arquivo `config/scene.cfg`, objetos automaticamente carregam seus materiais:
```
objetos/cube.obj        2.0   0.0   0.0   0.0   0.0   0.0   1.0   1.0   1.0        1
objetos/ground.obj      0.0  -1.0   0.0  90.0   0.0   0.0  10.0  10.0   1.0        0
```

### Controles do Visualizador
- **WASD/Setas**: Movimentação da câmera
- **Mouse**: Rotação da câmera
- **Espaço**: Disparo de projéteis
- **ESC**: Sair

O sistema de materiais melhora significativamente o realismo visual, proporcionando diferentes aparências para cada tipo de objeto na cena 3D.