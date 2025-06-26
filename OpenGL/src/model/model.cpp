#include "model.h"
#include <iostream>

Model::Model(const std::string& path, bool gamma) : gammaCorrection(gamma) {
  LoadModel(path);
}

void Model::Draw(Shader& shader) {
  for (auto& mesh : meshes) {
    mesh.Draw(shader);
  }
}

void Model::LoadModel(const std::string& path) {
  // Чтение файла с помощью Assimp
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      path,
      aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |  // ← уже есть
          aiProcess_GenSmoothNormals                                            // ← добавь это!
  );

  // Проверка на ошибки
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)  // если НЕ 0
  {
    std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }

  // Получение пути к файлу
  directory = path.substr(0, path.find_last_of('/'));

  // Рекурсивная обработка корневого узла Assimp
  ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
  if (!node || !scene) {
    std::cerr << "Null node or scene passed to ProcessNode!" << std::endl;
    return;
  }

  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    unsigned int meshIndex = node->mMeshes[i];
    if (meshIndex >= scene->mNumMeshes) {
      std::cerr << "Mesh index out of range: " << meshIndex << std::endl;
      continue;
    }

    aiMesh* mesh = scene->mMeshes[meshIndex];
    if (!mesh) {
      std::cerr << "Null mesh pointer at index: " << meshIndex << std::endl;
      continue;
    }

    meshes.push_back(ProcessMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
  // Данные для заполнения
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  // Цикл по всем вершинам меша
  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3
        vector;  // мы объявляем промежуточный вектор, т.к. Assimp использует свой собственный
                 // векторный класс, который не преобразуется напрямую в тип glm::vec3, поэтому
                 // сначала мы передаем данные в этот промежуточный вектор типа glm::vec3

    // Координаты
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    // Нормали
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.normal = vector;

    // Текстурные координаты
    if (mesh->mTextureCoords[0])  // если меш содержит текстурные координаты
    {
      glm::vec2 vec;

      // Вершина может содержать до 8 различных текстурных координат. Мы предполагаем, что мы не
      // будем использовать модели, в которых вершина может содержать несколько текстурных
      // координат, поэтому мы всегда берем первый набор (0)
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = 1.0f - mesh->mTextureCoords[0][i].y;
      vertex.tex_coords = vec;
    } else
      vertex.tex_coords = glm::vec2(0.0f, 0.0f);

    // Касательный вектор
    vector.x = mesh->mTangents[i].x;
    vector.y = mesh->mTangents[i].y;
    vector.z = mesh->mTangents[i].z;
    vertex.tangent = vector;

    // Вектор бинормали
    vector.x = mesh->mBitangents[i].x;
    vector.y = mesh->mBitangents[i].y;
    vector.z = mesh->mBitangents[i].z;
    vertex.bitangent = vector;
    vertices.push_back(vertex);
  }
  // Теперь проходимся по каждой грани меша (грань - это треугольник меша) и извлекаем
  // соответствующие индексы вершин
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    // Получаем все индексы граней и сохраняем их в векторе indices
    for (unsigned int j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
  }

  // Обрабатываем материалы
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

  // Мы вводим соглашение об именах сэмплеров в шейдерах. Каждая диффузная текстура будет называться
  // 'texture_diffuseN', где N - порядковый номер от 1 до MAX_SAMPLER_NUMBER. Тоже самое относится и
  // к другим текстурам: диффузная - texture_diffuseN отражения - texture_specularN нормали -
  // texture_normalN

  // 1. Диффузные карты
  std::vector<Texture> diffuseMaps =
      LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  // 2. Карты отражения
  std::vector<Texture> specularMaps =
      LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  // 3. Карты нормалей
  std::vector<Texture> normalMaps =
      LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

  // 4. Карты высот
  std::vector<Texture> heightMaps =
      LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
  textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

  // Возвращаем меш-объект, созданный на основе полученных данных
  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                 std::string typeName) {
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    std::string path = str.C_Str();
    std::cout << "[Load] type: " << typeName << ", path: " << str.C_Str() << std::endl;

    // Проверяем, есть ли текстура в кэше
    auto it = texture_cache.find(path);
    if (it != texture_cache.end()) {
      // Уже загружена — просто добавляем в текущий список
      textures.push_back(it->second);
    } else {
      // Загружаем текстуру и кэшируем
      Texture texture;
      texture.id = TextureFromFile(path.c_str(), directory);
      texture.type = typeName;
      texture.path = path;

      textures.push_back(texture);
      texture_cache[path] = texture;
    }
  }

  return textures;
}

GLuint TextureFromFile(const char* path, const std::string& directory, bool gamma) {
  std::string filename = directory + '/' + std::string(path);

  GLuint textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (!data) {
    std::cerr << "❌ Failed to load texture: " << filename << std::endl;
    return 0;
  }

  GLenum format;
  GLenum internalFormat;
  if (nrComponents == 1)
    format = internalFormat = GL_RED;
  else if (nrComponents == 3) {
    format = GL_RGB;
    internalFormat = gamma ? GL_SRGB : GL_RGB;
  } else if (nrComponents == 4) {
    format = GL_RGBA;
    internalFormat = gamma ? GL_SRGB_ALPHA : GL_RGBA;
  } else {
    std::cerr << "❌ Unsupported number of channels: " << nrComponents << std::endl;
    stbi_image_free(data);
    return 0;
  }

  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);
  return textureID;
}
