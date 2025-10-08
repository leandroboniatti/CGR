# Instruções para adicionar texturas

Para adicionar texturas aos modelos, coloque arquivos de imagem (.jpg, .png) na pasta `textures/` e crie arquivos .mtl correspondentes.

## Exemplo de arquivo .mtl (models/cube.mtl):

```
newmtl Material
Ka 0.2 0.2 0.2
Kd 0.8 0.8 0.8
Ks 1.0 1.0 1.0
Ns 32.0
map_Kd textures/default.jpg
```

## Para referenciar o material no arquivo .obj, adicione:

```
mtllib cube.mtl
usemtl Material
```

## Texturas de exemplo recomendadas:
- default.jpg (512x512) - Textura padrão
- wood.jpg - Para objetos de madeira
- metal.jpg - Para objetos metálicos
- brick.jpg - Para paredes
- grass.jpg - Para o chão

## Formatos suportados:
- JPEG (.jpg, .jpeg)
- PNG (.png)
- BMP (.bmp)
- TGA (.tga)