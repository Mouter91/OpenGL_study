#include "config/config.h"
#include "shader/shader.h"

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

  Shader lightingShader("../src/shader/vertex_box.sh", "../src/shader/fragment_box.sh");
  Shader lampShader("../src/shader/vertex_lamp.sh", "../src/shader/fragment_lamp.sh");

  float vertices[] = {
      // координаты        // нормали           // текстурные координаты
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, -1.0f,
      0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f,
      0.0f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 1.0f,
      0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,
      0.0f,  0.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 0.0f,
      -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f,  0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  GLuint VBO, cubeVAO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Координатные атрибуты
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  GLuint lightVAO;
  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  GLuint diffMap = LoadTexture("../src/texture/wooden_container_2.png");
  GLuint specMap = LoadTexture("../src/texture/container_2_specular.png");

  lightingShader.use();
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

  // Раскомментируйте следующую строку для отрисовки полигонов в режиме каркаса
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    TimeControlConfig::deltaTime = currentFrame - TimeControlConfig::lastFrame;
    TimeControlConfig::lastFrame = currentFrame;

    key_bind.process();
    key_bind.processCamera(TimeControlConfig::deltaTime);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Отрисовка куба
    lightingShader.use();

    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("viewPos", camera.GetPosition());

    // Свойства света
    lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // Свойства материалов
    lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 64.0f);

    const glm::mat4 &view = camera.GetViewMatrix();
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.GetZoom()),
                         (float)WindowConfig::width / (float)WindowConfig::height, 0.1f, 100.0f);

    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    lightingShader.setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specMap);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Отрисовка лампочки
    lampShader.use();
    lampShader.setMat4("projection", projection);
    lampShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));  // куб меньшего размера
    lampShader.setMat4("model", model);

    glBindVertexArray(lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();

  return 0;
}
