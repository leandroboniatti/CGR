# Copilot Instructions - CGR: Visualizador 3D (Grau A)

## Project Overview
This is a 3D model visualizer implementing a First Person Shooter-style application using OpenGL 4+ with custom OBJ loader, collision detection, and bullet reflection system. Built for academic computer graphics coursework.

## Architecture & Key Patterns

### Project Structure
The project follows this established structure:
- `/src/` - Core C++ classes: Camera, Obj3D, Scene, Bullet, ShaderProgram
- `/shaders/` - GLSL 4.0+ shaders for rendering pipeline
- `/config/` - Scene configuration files (scene.cfg format)
- `/models/` - OBJ/MTL files and textures
- `main.cpp` - GLFW application entry point with input handling

### Core Classes Architecture
- **Scene**: Manages objects collection, bullet system, collision detection
- **Obj3D**: OBJ loader with MTL support, VAO/VBO management per group
- **Camera**: FPS-style camera with mouse look and WASD movement
- **Bullet**: Projectile physics with collision and reflection
- **ShaderProgram**: GLSL shader compilation and uniform management

### Rendering Pipeline
- OpenGL 4.0+ core profile with GLAD
- Vertex layout: position(vec3), normal(vec3), texCoords(vec2)
- Phong lighting model with ambient/diffuse/specular components
- Separate shader programs for objects and bullets
- One VAO per OBJ group for efficient rendering

## Development Workflow

### Build System
- Uses Makefile (no CMake as specified in requirements)
- Compile with: `make`, run with: `./visualizador3d.exe`
- Dependencies: GLFW3, GLAD (included), GLM, OpenGL 4.0+ drivers

### OBJ Loading Implementation
- Custom parser in `Obj3D::loadOBJ()` - maintains vertex indexing per specification
- Automatic face triangulation for 4+ vertex faces using fan triangulation
- Material loading via MTL files with texture support
- Bounding box calculation for collision detection

### FPS Controls Implementation
```cpp
// Camera controls (in main.cpp processInput):
// W/↑ - camera->processKeyboard(FORWARD, deltaTime)
// S/↓ - camera->processKeyboard(BACKWARD, deltaTime)  
// A/← - camera->processKeyboard(LEFT, deltaTime)
// D/→ - camera->processKeyboard(RIGHT, deltaTime)
// SPACE - scene->shoot(camera->getPosition(), camera->getFront())
```

## Code Conventions

### Object Management
- Use `std::unique_ptr` for automatic memory management
- Each OBJ group gets its own VAO/VBO/EBO as specified
- Matrix transformations stored in `Obj3D::modelMatrix`
- Eliminable property configurable per object in scene.cfg

### Collision Detection Pattern
```cpp
// Ray-AABB intersection in Obj3D::checkRayIntersection()
// Bullet reflection: R = D - 2(D·N)N in Bullet::reflect()
// Scene collision checking in Scene::checkCollisions()
```

### Scene Configuration Format
```
# scene.cfg format:
obj_path pos_x pos_y pos_z rot_x rot_y rot_z scale_x scale_y scale_z eliminable(0/1)
models/cube.obj 2.0 0.0 0.0 0.0 0.0 0.0 1.0 1.0 1.0 1
```

## Key Implementation Details

### Vertex Data Structure
```cpp
struct Vertex {
    glm::vec3 position;  // Required
    glm::vec3 normal;    // For lighting
    glm::vec2 texCoords; // For texture mapping
};
```

### Shader Uniform Patterns
- Model/View/Projection matrices in vertex shaders
- Material properties (ambient/diffuse/specular/shininess) in fragment shaders
- Light position/color and view position for Phong lighting
- Texture samplers bound to GL_TEXTURE0

### Memory Management
- GLM library used for all vector/matrix operations as specified
- GLAD for OpenGL function loading (included in project)
- STB_IMAGE for texture loading (single header implementation)
- Proper OpenGL resource cleanup in destructors
- RAII pattern for GPU resources

## Common Workflows

### Adding New 3D Objects
1. Place OBJ/MTL files in `/models/` directory
2. Add entry to `config/scene.cfg` with position/rotation/scale/eliminable flag
3. Textures referenced in MTL files should be in same directory as OBJ

### Modifying Shaders
- Edit `.glsl` files in `/shaders/` directory
- Shader hot-reloading not implemented - requires recompile
- Vertex shader: handle MVP transforms and pass data to fragment
- Fragment shader: implement Phong lighting with material properties

### Debugging Collision Issues
- Bounding boxes calculated in `Obj3D::calculateBoundingBox()`
- Enable wireframe mode or debug rendering as needed
- Check transformed coordinates in collision detection functions

---
*This project implements all specified requirements: OBJ loading with triangulation, texture mapping, FPS camera controls, shooting mechanics with collision detection and reflection, configurable eliminable objects, and bounding box collision detection.*