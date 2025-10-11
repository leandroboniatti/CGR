#ifndef PROJETIL_H
#define PROJETIL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"


class Projetil {
public:
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    float lifetime;
    float maxLifetime;
    bool  active;
    
    // Renderização
    unsigned int VAO, VBO;
    
    Projetil();

    // Construtor com parâmetros
    Projetil(const glm::vec3& startPos, const glm::vec3& dir, float projetilSpeed = 5.0f, float maxLife = 5.0f);
    
    ~Projetil();
    
    // Atualiza a posição do projétil e verifica se deve ser desativado
    void update(float deltaTime);

    // Renderiza o projétil
    void draw(const Shader& shader) const;

    bool isActive() const { return active && lifetime < maxLifetime; }

    // Calcula a direção do vetor de reflexão
    void reflect(const glm::vec3& normal);

    void desativar() { active = false; }
    
    void setupMesh();

    void cleanup();
};

#endif