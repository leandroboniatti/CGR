#include "Camera.h"

Camera::Camera(glm::vec3 position) : m_position(position) {}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::mat4 Camera::projectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(m_fov), aspect, 0.1f, 100.0f);
}

void Camera::processKeyboard(char key, float dt) {
    float vel = m_speed * dt;
    glm::vec3 right = glm::normalize(glm::cross(m_front, m_up));
    if (key == 'W') m_position += m_front * vel;
    if (key == 'S') m_position -= m_front * vel;
    if (key == 'A') m_position -= right * vel;
    if (key == 'D') m_position += right * vel;
}

void Camera::processMouse(float xoffset, float yoffset) {
    xoffset *= m_sensitivity;
    yoffset *= m_sensitivity;
    m_yaw += xoffset;
    m_pitch += yoffset;
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;
    updateVectors();
}

void Camera::processScroll(float yoffset) {
    m_fov -= yoffset;
    if (m_fov < 10.0f) m_fov = 10.0f;
    if (m_fov > 90.0f) m_fov = 90.0f;
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
}
