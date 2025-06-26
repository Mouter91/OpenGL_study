#include "config/config.h"
#include "shader/shader.h"
#include "model/model.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WindowConfig::width, WindowConfig::height, WindowConfig::title, NULL, NULL);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *, int width, int height) { glViewport(0, 0, width, height); });

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  InputHandler key_bind(window);

  Camera camera;
  key_bind.BindCamera(&camera);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetWindowUserPointer(window, &camera);

  glfwSetCursorPosCallback(window, [](GLFWwindow *win, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = static_cast<float>(WindowConfig::width) / 2,
                 lastY = static_cast<float>(WindowConfig::height) / 2;

    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // y координата перевернута

    lastX = xpos;
    lastY = ypos;

    auto *cam = static_cast<Camera *>(glfwGetWindowUserPointer(win));
    if (cam) {
      cam->ProcessMouseMovement(xoffset, yoffset);
    }
  });

  glfwSetScrollCallback(window, [](GLFWwindow *win, double xoffset, double yoffset) {
    auto *cam = static_cast<Camera *>(glfwGetWindowUserPointer(win));
    if (cam) {
      cam->ProcessMouseScroll(yoffset);
    }
  });

  Shader ourShader("../src/source/ShProgram/vertex_model.sh",
                   "../src/source/ShProgram/fragment_model.sh");

  Model ourModel("../src/source/Model/backpack/backpack.obj");

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    TimeControlConfig::deltaTime = currentFrame - TimeControlConfig::lastFrame;
    TimeControlConfig::lastFrame = currentFrame;

    key_bind.process();
    key_bind.processCamera(TimeControlConfig::deltaTime);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Отрисовка куба
    ourShader.use();

    const glm::mat4 &view = camera.GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.GetZoom()),
                         (float)WindowConfig::width / (float)WindowConfig::height, 0.1f, 100.0f);

    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,
                           glm::vec3(0.0f, 0.0f, 0.0f));  // смещаем вниз чтобы быть в центре сцены
    model = glm::scale(
        model,
        glm::vec3(1.0f, 1.0f,
                  1.0f));  // объект слишком большой для нашей сцены, поэтому немного уменьшим его
    ourShader.setMat4("model", model);
    ourModel.Draw(ourShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
