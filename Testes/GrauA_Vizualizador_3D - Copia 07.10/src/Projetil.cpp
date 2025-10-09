#include "Projetil.h"
#include <glad/glad.h>
#include <iostream>


Projetil::Projetil() 
    : position(0.0f), direction(0.0f, 0.0f, 1.0f), speed(50.0f), 
      lifetime(0.0f), maxLifetime(5.0f), active(false), VAO(0), VBO(0) {
    setupMesh();
}

Projetil::Projetil(const glm::vec3& startPos, const glm::vec3& dir, float projetilSpeed, float maxLife)
    : position(startPos), direction(glm::normalize(dir)), speed(projetilSpeed),
      lifetime(0.0f), maxLifetime(maxLife), active(true), VAO(0), VBO(0) {
    setupMesh();
}

Projetil::~Projetil() {
    cleanup();  // libera recursos da OpenGL
}

void Projetil::update(float deltaTime) {
    if (!active) return;
    // Atualiza a posição do projétil e verifica se deve ser desativado
    position += direction * speed * deltaTime;
    lifetime += deltaTime;
    
    if (lifetime >= maxLifetime) {
        active = false;
    }
}

void Projetil::draw(const Shader& shader) const {
    if (!active || VAO == 0) return;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(0.05f)); // projétil pequeno - ajuste conforme necessário
    
    shader.setMat4("model", model);
    shader.setVec3("objectColor", 1.0f, 1.0f, 0.0f); // Projétil amarelo

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36); // Cubo tem 36 vértices
    glBindVertexArray(0);
}

void Projetil::reflect(const glm::vec3& normal) {
    // calcula a direção do vetor de reflexão
    direction = direction - 2.0f * glm::dot(direction, normal) * normal;
    direction = glm::normalize(direction);
}

void Projetil::setupMesh() {
    // Para visualização de projétil, usamos um cubo simples - 36 vértices
    float vertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Projetil::cleanup() {    // libera recursos da OpenGL
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
}