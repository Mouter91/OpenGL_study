#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace WindowConfig {
inline constexpr unsigned int width = 800;
inline constexpr unsigned int height = 600;
inline const char* title = "OpenGL window";
}  // namespace WindowConfig
   //

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

  // Пример метода
  void process() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

 private:
  GLFWwindow* window;
};
