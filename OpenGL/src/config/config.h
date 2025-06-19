#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "stb_image.h"
#include "../camera/camera.h"

namespace WindowConfig {
inline constexpr unsigned int width = 800;
inline constexpr unsigned int height = 600;
inline const char* title = "OpenGL window";
}  // namespace WindowConfig

namespace TimeControlConfig {
inline float deltaTime = 0.0f;  // время между текущим и последним кадрами
inline float lastFrame = 0.0f;  // время последнего кадра
}  // namespace TimeControlConfig

class InputHandler {
 public:
  // Запрещаем конструктор по умолчанию
  InputHandler() = delete;

  // Запрещаем копирование
  InputHandler(const InputHandler&) = delete;
  InputHandler& operator=(const InputHandler&) = delete;

  // Разрешаем только создание с окном
  explicit InputHandler(GLFWwindow* win) : window(win) {
  }

  // Можно оставить перемещение запрещённым
  InputHandler(InputHandler&&) = delete;
  InputHandler& operator=(InputHandler&&) = delete;

  ~InputHandler() = default;

  void BindCamera(Camera* cam) {
    camera = cam;
  }

  // Пример метода
  void process() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

  void processCamera(float deltaTime) {
    if (!camera)
      return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      camera->ProcessKeyboard(Camera::Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      camera->ProcessKeyboard(Camera::Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      camera->ProcessKeyboard(Camera::Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      camera->ProcessKeyboard(Camera::Movement::RIGHT, deltaTime);
  }

 private:
  GLFWwindow* window;
  Camera* camera = nullptr;
};

GLuint LoadTexture(const std::string& path);
