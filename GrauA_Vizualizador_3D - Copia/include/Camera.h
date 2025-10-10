#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;   // ângulo Yaw inicial da câmera
const float PITCH = 0.0f;   // ângulo Pitch inicial da câmera
const float SPEED = 2.5f;   // velocidade de movimento da câmera
const float SENSITIVITY = 0.1f;   // sensibilidade do mouse
const float ZOOM = 45.0f;   // nível de zoom da câmera

class Camera {
public:
    // Atributos da câmera
    glm::vec3 Position; // posição da câmera no mundo
    glm::vec3 Front; // direção para onde a câmera está olhando
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    // Ângulos de Euler
    float Yaw;
    float Pitch;
    
    // Opções da câmera
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    
    // Construtor padrão com valores iniciais
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw = YAW, float pitch = PITCH);
    
    // Retorna a matriz de visualização calculada usando ângulos de Euler e a matriz LookAt
    glm::mat4 GetViewMatrix();
    
    // Processa o input recebido do teclado
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    
    // Processa o input recebido do mouse
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    
    // Processa o input recebido de um evento de rolagem do mouse
    void ProcessMouseScroll(float yoffset);
    
    // Calcula o vetor front a partir dos ângulos de Euler (atualizados) da câmera
    void updateCameraVectors();
};

#endif