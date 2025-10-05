# Sistema First Person Shooter - Guia de Uso

## 🎮 Controles do Jogo

### Movimento da Câmera
- **W** ou **↑**: Move a câmera para frente
- **S** ou **↓**: Move a câmera para trás  
- **A** ou **←**: Rotaciona a câmera para a esquerda
- **D** ou **→**: Rotaciona a câmera para a direita

### Sistema de Tiro
- **BARRA DE ESPAÇO**: Atira um projétil na direção da câmera

### Saída
- **ESC**: Fecha o programa

## 🎯 Mecânicas de Jogo

### Projéteis
- Os projéteis são disparados na direção que a câmera está olhando
- Possuem uma velocidade de 50 unidades por segundo
- Têm um tempo de vida de 5 segundos
- São renderizados como pequenas esferas amarelas
- Existe um cooldown de 0.3 segundos entre disparos

### Sistema de Colisão
- Cada objeto possui um **bounding box** para detecção de colisão
- O sistema usa **colisão esférica** para detecção mais rápida e eficiente
- O raio de colisão é calculado automaticamente baseado no tamanho do objeto

### Objetos Elimináveis vs Não-Elimináveis

#### Objetos Elimináveis (eliminable = 1)
- **Comportamento**: Quando atingidos por um projétil, são **removidos da cena**
- **Exemplo**: Cubo, Esfera (conforme config.txt)
- **Efeito**: O projétil também é destruído no impacto

#### Objetos Não-Elimináveis (eliminable = 0)  
- **Comportamento**: Quando atingidos, **refletem o projétil**
- **Exemplo**: Pirâmide, Parede (conforme config.txt)
- **Reflexão**: O projétil ricocheia usando a normal da superfície
- **Efeito**: O projétil continua com nova direção

## ⚙️ Configuração de Objetos

O arquivo `assets/config.txt` define os objetos da cena:

```
# Formato: arquivo_obj pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminavel
assets/models/cubo.obj 0.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1
assets/models/esfera.obj 3.0 0.0 -2.0 0.0 0.0 0.0 0.5 0.5 0.5 1
assets/models/piramide.obj -3.0 0.0 -2.0 0.0 45.0 0.0 1.0 1.0 1.0 0
assets/models/parede.obj 0.0 0.0 -5.0 0.0 0.0 0.0 10.0 3.0 0.5 0
```

### Parâmetros:
- **arquivo_obj**: Caminho para o modelo 3D
- **pos_x, pos_y, pos_z**: Posição no mundo 3D
- **rot_x, rot_y, rot_z**: Rotação em graus
- **scale_x, scale_y, scale_z**: Escala do objeto
- **eliminavel**: 1 = eliminável, 0 = não-eliminável

## 🔧 Recursos Implementados

### Sistema de Colisão Avançado
- **Ray-AABB intersection** para bounding boxes
- **Colisão esférica** para detecção rápida
- **Cálculo de normais** para reflexão precisa
- **Detecção do objeto mais próximo** para múltiplas colisões

### Sistema de Reflexão Física
- Utiliza a fórmula: `reflected = incident - 2 * dot(incident, normal) * normal`
- Calcula a normal correta da superfície de colisão
- Previne colisões contínuas movendo o projétil após reflexão

### Otimizações
- **Frustum culling** básico (objetos invisíveis não são processados)
- **Cleanup automático** de projéteis expirados
- **Memory management** adequado para objetos dinâmicos

## 🎪 Cenário de Teste

O cenário padrão inclui:
- **Cubo** (eliminável) no centro
- **Esfera** (eliminável) à direita  
- **Pirâmide** (não-eliminável) à esquerda
- **Parede** (não-eliminável) ao fundo

Experimente atirar nos diferentes objetos para ver os comportamentos distintos!

## 🐛 Troubleshooting

### Projeto não compila
- Verifique se todas as dependências estão instaladas
- Certifique-se que o MinGW está no PATH
- Execute a task "build" no VS Code

### Projéteis não aparecem
- Verifique se os shaders estão carregados corretamente
- Confirme que o OpenGL está funcionando

### Colisões não funcionam
- Verifique se os modelos OBJ têm bounding boxes válidos
- Confirme que os objetos estão sendo carregados corretamente

## 📊 Estatísticas do Sistema

- **Velocidade de projétil**: 50 unidades/segundo
- **Tempo de vida**: 5 segundos
- **Cooldown de tiro**: 0.3 segundos
- **Velocidade de câmera**: 5 unidades/segundo
- **Sensibilidade de rotação**: 0.1 graus/pixel

---

**Divirta-se explorando o sistema FPS! 🚀**