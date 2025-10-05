#pragma once

// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB Image
#include "stb_image.h"

// Standard Library
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

// Project headers
#include "Face.h"
#include "Material.h"
#include "Group.h"
#include "OBJ3D.h"
#include "OBJReader.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Bullet.h"
#include "System.h"

// Macros úteis
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// Funções auxiliares para debug do OpenGL
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);