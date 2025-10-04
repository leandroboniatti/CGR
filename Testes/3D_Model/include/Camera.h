#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position = {0.0f, 0.0f, 3.0f});
    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspect) const;

    void processKeyboard(char key, float deltaTime);
    void processMouse(float xoffset, float yoffset);
    void processScroll(float yoffset);

    glm::vec3 position() const { return m_position; }
    float fov() const { return m_fov; }

private:
    glm::vec3 m_position;
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    float m_speed = 5.0f;
    float m_sensitivity = 0.1f;
    float m_fov = 60.0f;
    void updateVectors();
};
