#include "Bullet.h"
#include <glad/glad.h>  // biblioteca de funções baseada nas definições/especificações OPENGL - Incluir antes de outros que requerem OpenGL (como GLFW)
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Bullet::Bullet(const glm::vec3& startPos, const glm::vec3& dir, float bulletSpeed, float maxLife)
    : position(startPos), direction(glm::normalize(dir)), speed(bulletSpeed), 
      lifetime(0.0f), maxLifetime(maxLife), active(true) {}

void Bullet::update(float deltaTime) {
    if (!active) return;
    
    // Atualizar posição
    position += direction * speed * deltaTime;
    
    // Atualizar tempo de vida
    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        active = false;
    }
}

void Bullet::render(unsigned int shaderProgram) {
    if (!active) return;
    
    // Renderizar como uma pequena esfera ou cubo
    // Por simplicidade, vamos usar um cubo pequeno
    
    static unsigned int VAO = 0;
    static bool initialized = false;
    
    if (!initialized) {
        // Vértices de um cubo pequeno
        float vertices[] = {
            -0.05f, -0.05f, -0.05f,
             0.05f, -0.05f, -0.05f,
             0.05f,  0.05f, -0.05f,
             0.05f,  0.05f, -0.05f,
            -0.05f,  0.05f, -0.05f,
            -0.05f, -0.05f, -0.05f,

            -0.05f, -0.05f,  0.05f,
             0.05f, -0.05f,  0.05f,
             0.05f,  0.05f,  0.05f,
             0.05f,  0.05f,  0.05f,
            -0.05f,  0.05f,  0.05f,
            -0.05f, -0.05f,  0.05f,

            -0.05f,  0.05f,  0.05f,
            -0.05f,  0.05f, -0.05f,
            -0.05f, -0.05f, -0.05f,
            -0.05f, -0.05f, -0.05f,
            -0.05f, -0.05f,  0.05f,
            -0.05f,  0.05f,  0.05f,

             0.05f,  0.05f,  0.05f,
             0.05f,  0.05f, -0.05f,
             0.05f, -0.05f, -0.05f,
             0.05f, -0.05f, -0.05f,
             0.05f, -0.05f,  0.05f,
             0.05f,  0.05f,  0.05f,

            -0.05f, -0.05f, -0.05f,
             0.05f, -0.05f, -0.05f,
             0.05f, -0.05f,  0.05f,
             0.05f, -0.05f,  0.05f,
            -0.05f, -0.05f,  0.05f,
            -0.05f, -0.05f, -0.05f,

            -0.05f,  0.05f, -0.05f,
             0.05f,  0.05f, -0.05f,
             0.05f,  0.05f,  0.05f,
             0.05f,  0.05f,  0.05f,
            -0.05f,  0.05f,  0.05f,
            -0.05f,  0.05f, -0.05f
        };
        
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);
        initialized = true;
    }
    
    // Configurar matriz de modelo
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    
    glUseProgram(shaderProgram);
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    
    // Cor do projétil (amarelo)
    int colorLoc = glGetUniformLocation(shaderProgram, "bulletColor");
    glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Bullet::reflect(const glm::vec3& normal) {
    // Reflexão: R = D - 2(D·N)N
    direction = direction - 2.0f * glm::dot(direction, normal) * normal;
    direction = glm::normalize(direction);
}