#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    // Posição e orientação
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    // Ângulos de Euler
    float yaw;
    float pitch;

    // Opções da câmera
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // Matrizes
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

public:
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yawAngle = -90.0f, 
           float pitchAngle = 0.0f);
    ~Camera();

    // Getters
    const glm::vec3& getPosition() const;
    const glm::vec3& getFront() const;
    const glm::vec3& getUp() const;
    const glm::vec3& getRight() const;
    float getYaw() const;
    float getPitch() const;
    float getZoom() const;
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;

    // Setters
    void setPosition(const glm::vec3& pos);
    void setMovementSpeed(float speed);
    void setMouseSensitivity(float sensitivity);
    void setZoom(float zoomLevel);

    // Movimento
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);

    // Rotação
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);

    // Atualização das matrizes
    void updateViewMatrix();
    void updateProjectionMatrix(float aspectRatio, float nearPlane = 0.1f, float farPlane = 100.0f);

private:
    void updateCameraVectors();
};