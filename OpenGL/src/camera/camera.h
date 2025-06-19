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
    updateViewMatrix();
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

    updateViewMatrix();
  }

  const glm::mat4& GetViewMatrix() const {
    return view;
  }

 private:
  glm::mat4 view;
  glm::vec3 cameraPos;
  glm::vec3 cameraFront;
  glm::vec3 cameraUp;

  float cameraSpeed;

  void updateViewMatrix() {
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  }
};
