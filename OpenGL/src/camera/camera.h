#pragma once

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
 public:
  Camera()
      : cameraPos(0.0f, 0.0f, 3.0f),
        cameraFront(0.0f, 0.0f, -1.0f),
        cameraUp(0.0f, 1.0f, 0.0f),
        cameraSpeed(2.5f) {
    updateViewMatrixKey();
  }

  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;
  Camera(Camera&&) = default;
  Camera& operator=(Camera&&) = default;

  ~Camera() = default;

  enum class Movement { FORWARD, BACKWARD, LEFT, RIGHT };

  void SetCameraSpeed(float speed) {
    cameraSpeed = speed;
  }

  float GetCameraSpeed() const {
    return cameraSpeed;
  }

  float GetZoom() const {
    return zoom;
  }

  const glm::vec3 GetPosition() const {
    return cameraPos;
  }

  const glm::vec3 GetFrontDirection() const {
    return cameraFront;
  }

  void ProcessKeyboard(Movement direction, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;
    if (direction == Movement::FORWARD)
      cameraPos += cameraFront * velocity;
    if (direction == Movement::BACKWARD)
      cameraPos -= cameraFront * velocity;
    if (direction == Movement::LEFT)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
    if (direction == Movement::RIGHT)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;

    updateViewMatrixKey();
  }

  const glm::mat4& GetViewMatrix() const {
    return view;
  }

  void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
    xoffset *= mouseSens;
    yoffset *= mouseSens;

    yaw += xoffset;
    pitch += yoffset;

    // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
    if (constrainPitch) {
      if (pitch > 89.0f)
        pitch = 89.0f;
      if (pitch < -89.0f)
        pitch = -89.0f;
    }

    // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные
    // значения углов Эйлера
    updateViewMatrixMouse();
  }

  void ProcessMouseScroll(float yoffset) {
    if (zoom >= 1.0f && zoom <= 45.0f)
      zoom -= yoffset;
    if (zoom <= 1.0f)
      zoom = 1.0f;
    if (zoom >= 45.0f)
      zoom = 45.0f;
  }

 private:
  glm::mat4 view;
  glm::vec3 cameraPos;
  glm::vec3 cameraFront;
  glm::vec3 cameraUp;

  float cameraSpeed = 2.5f;
  float mouseSens = 0.1f;
  float zoom = 45.0f;

  float yaw = -90.0f;
  float pitch = 0.0f;

  void updateViewMatrixKey() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }

  void updateViewMatrixMouse() {
    glm::vec3 front;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }
};
