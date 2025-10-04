cmake_minimum_required(VERSION 3.21)
project(OBJViewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8
)

# GLAD (v2)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v2.0.5
)

# GLM (header-only)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)


# stb (header-only, repo sem CMakeLists)
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)

FetchContent_MakeAvailable(glfw glad glm stb)


# Garantir alvos interface para glm e stb se não definidos (defensivo)
if(NOT TARGET glm)
  add_library(glm INTERFACE)
  target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
endif()
if(NOT TARGET stb)
  add_library(stb INTERFACE)
  target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

add_executable(obj_viewer
    src/main.cpp
    src/App.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Model.cpp
    src/Texture.cpp
    src/Renderer.cpp
)

target_include_directories(obj_viewer PRIVATE include)
target_include_directories(obj_viewer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(obj_viewer PRIVATE ${glad_SOURCE_DIR}/include)

target_link_libraries(obj_viewer PRIVATE glfw glad stb glm)
if (WIN32)
  target_link_libraries(obj_viewer PRIVATE opengl32)
endif()

if (WIN32)
    target_compile_definitions(obj_viewer PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(obj_viewer PRIVATE ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

if (MSVC)
    target_compile_options(obj_viewer PRIVATE /W4 /permissive-)
else()
    target_compile_options(obj_viewer PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_custom_target(run
    COMMAND obj_viewer
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS obj_viewer
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
