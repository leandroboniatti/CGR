#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yawAngle, float pitchAngle)
    : position(pos)
    , worldUp(up)
    , yaw(yawAngle)
    , pitch(pitchAngle)
    , movementSpeed(2.5f)
    , mouseSensitivity(0.1f)
    , zoom(45.0f)
    , viewMatrix(1.0f)
    , projectionMatrix(1.0f) {
    updateCameraVectors();
    updateViewMatrix();
}

Camera::~Camera() {
}

const glm::vec3& Camera::getPosition() const {
    return position;
}

const glm::vec3& Camera::getFront() const {
    return front;
}

const glm::vec3& Camera::getUp() const {
    return up;
}

const glm::vec3& Camera::getRight() const {
    return right;
}

float Camera::getYaw() const {
    return yaw;
}

float Camera::getPitch() const {
    return pitch;
}

float Camera::getZoom() const {
    return zoom;
}

const glm::mat4& Camera::getViewMatrix() const {
    return viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

void Camera::setPosition(const glm::vec3& pos) {
    position = pos;
    updateViewMatrix();
}

void Camera::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

void Camera::setMouseSensitivity(float sensitivity) {
    mouseSensitivity = sensitivity;
}

void Camera::setZoom(float zoomLevel) {
    zoom = glm::clamp(zoomLevel, 1.0f, 90.0f);
}

void Camera::moveForward(float deltaTime) {
    position += front * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveBackward(float deltaTime) {
    position -= front * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveLeft(float deltaTime) {
    position -= right * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveRight(float deltaTime) {
    position += right * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveUp(float deltaTime) {
    position += up * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::moveDown(float deltaTime) {
    position -= up * movementSpeed * deltaTime;
    updateViewMatrix();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    updateCameraVectors();
    updateViewMatrix();
}

void Camera::processMouseScroll(float yoffset) {
    zoom -= yoffset;
    zoom = glm::clamp(zoom, 1.0f, 90.0f);
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::updateProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) {
    projectionMatrix = glm::perspective(glm::radians(zoom), aspectRatio, nearPlane, farPlane);
}

void Camera::updateCameraVectors() {
    glm::vec3 frontVector;
    frontVector.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVector.y = sin(glm::radians(pitch));
    frontVector.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontVector);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}