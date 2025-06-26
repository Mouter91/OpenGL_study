#include "mesh.h"
#include <cstddef>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
           std::vector<Texture> textures) {
  vertices_ = vertices;
  indices_ = indices;
  textures_ = textures;

  SetupMesh();
}

void Mesh::Draw(Shader& shader) {
  GLuint diffuseNr = 1;
  GLuint specularNr = 1;
  GLuint normalNr = 1;
  GLuint heightNr = 1;

  for (int i = 0; i < textures_.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number;
    std::string name = textures_[i].type;

    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNr++);
    } else if (name == "texture_specular") {
      number = std::to_string(specularNr++);
    } else if (name == "texture_normal") {
      number = std::to_string(normalNr++);
    } else if (name == "texture_height") {
      number = std::to_string(heightNr++);
    }

    glUniform1i(glGetUniformLocation(shader.GetID(), (name + number).c_str()), i);

    glBindTexture(GL_TEXTURE_2D, textures_[i].id);
  }

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), &indices_[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  // Нормали вершин
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  // Текстурные координаты вершин
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, tex_coords));

  // Касательный вектор вершины
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

  // Вектор бинормали вершины
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void*)offsetof(Vertex, bitangent));

  glBindVertexArray(0);
}
