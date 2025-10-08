#include "Scene.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Scene::Scene() : lightPosition(5.0f, 5.0f, 5.0f), lightColor(1.0f, 1.0f, 1.0f) {
    // Carregar shaders
    if (!shaderProgram.loadShaders("shaders/vertex.glsl", "shaders/fragment.glsl")) {
        std::cerr << "Erro ao carregar shaders principais" << std::endl;
    }
    
    if (!bulletShader.loadShaders("shaders/bullet_vertex.glsl", "shaders/bullet_fragment.glsl")) {
        std::cerr << "Erro ao carregar shaders de projétil" << std::endl;
    }
    
    setupLighting();
}

Scene::~Scene() {
    objects.clear();
    bullets.clear();
}

bool Scene::loadFromConfig(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        std::cerr << "Não foi possível abrir arquivo de configuração: " << configPath << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Ignorar comentários e linhas vazias
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string objPath;
        glm::vec3 position, rotation, scale;
        bool eliminable;
        
        if (iss >> objPath >> position.x >> position.y >> position.z 
               >> rotation.x >> rotation.y >> rotation.z 
               >> scale.x >> scale.y >> scale.z >> eliminable) {
            
            auto object = std::make_unique<Obj3D>(objPath, position, rotation, scale, eliminable);
            if (object->load()) {
                addObject(std::move(object));
                std::cout << "Objeto carregado: " << objPath << std::endl;
            } else {
                std::cerr << "Falha ao carregar objeto: " << objPath << std::endl;
            }
        }
    }
    
    file.close();
    return true;
}

void Scene::addObject(std::unique_ptr<Obj3D> object) {
    objects.push_back(std::move(object));
}

void Scene::shoot(const glm::vec3& origin, const glm::vec3& direction) {
    auto bullet = std::make_unique<Bullet>(origin, direction);
    bullets.push_back(std::move(bullet));
}

void Scene::update(float deltaTime) {
    updateBullets(deltaTime);
    checkCollisions();
}

void Scene::updateBullets(float deltaTime) {
    // Atualizar projéteis
    for (auto& bullet : bullets) {
        if (bullet->isActive()) {
            bullet->update(deltaTime);
        }
    }
    
    // Remover projéteis inativos
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const std::unique_ptr<Bullet>& bullet) {
                return !bullet->isActive();
            }),
        bullets.end()
    );
}

void Scene::checkCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet->isActive()) continue;
        
        for (auto it = objects.begin(); it != objects.end();) {
            float distance;
            if ((*it)->checkRayIntersection(bullet->getPosition(), bullet->getDirection(), distance)) {
                if (distance < 0.1f) { // Tolerância para colisão
                    if ((*it)->isEliminable()) {
                        // Remover objeto eliminável
                        it = objects.erase(it);
                        bullet->setActive(false);
                        std::cout << "Objeto eliminado!" << std::endl;
                        break;
                    } else {
                        // Refletir projétil
                        glm::vec3 normal(0.0f, 1.0f, 0.0f); // Normal simplificada
                        bullet->reflect(normal);
                        std::cout << "Projétil refletido!" << std::endl;
                        ++it;
                    }
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
}

void Scene::render(const Camera& camera) {
    // Renderizar objetos 3D
    shaderProgram.use();
    
    // Configurar matrizes de câmera
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(1024.0f / 768.0f); // Aspect ratio
    
    shaderProgram.setMat4("view", glm::value_ptr(view));
    shaderProgram.setMat4("projection", glm::value_ptr(projection));
    
    // Configurar lighting
    shaderProgram.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
    shaderProgram.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
    shaderProgram.setVec3("viewPos", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
    
    // Renderizar todos os objetos
    for (const auto& object : objects) {
        object->render(shaderProgram.getID());
    }
    
    // Renderizar projéteis
    bulletShader.use();
    bulletShader.setMat4("view", glm::value_ptr(view));
    bulletShader.setMat4("projection", glm::value_ptr(projection));
    
    for (const auto& bullet : bullets) {
        bullet->render(bulletShader.getID());
    }
}

void Scene::setupLighting() {
    shaderProgram.use();
    shaderProgram.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
    shaderProgram.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
}