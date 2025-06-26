#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <unordered_map>

#include "stb_image.h"
#include "mesh.h"
#include "../shader/shader.h"

GLuint TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model {
 public:
  Model(const std::string &path, bool gamma = false);
  void Draw(Shader &shader);

 private:
  std::vector<Texture> textures_loaded;
  std::vector<Mesh> meshes;
  std::string directory;
  bool gammaCorrection;

  std::unordered_map<std::string, Texture> texture_cache;

  // Загружаем модель с помощью Assimp и сохраняем полученные меши в векторе meshes
  void LoadModel(const std::string &path);
  // Рекурсивная обработка узла. Обрабатываем каждый отдельный меш, расположенный в узле, и
  // повторяем этот процесс для своих дочерних углов (если таковы вообще имеются)
  void ProcessNode(aiNode *node, const aiScene *scene);
  Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                            std::string typeName);
};
