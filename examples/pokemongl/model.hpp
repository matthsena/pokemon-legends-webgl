#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "abcgOpenGL.hpp"

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Model {
public:
  bool m_hasNormals{false};

  void loadObj(std::string_view path, std::vector<Vertex> *m_vertices,
               std::vector<GLuint> *m_indices, GLuint *m_VBO, GLuint *m_EBO,
               bool standardize = true);

  void render(std::vector<GLuint> *m_indices, GLuint *m_VAO,
              int numTriangles = -1) const;

  void renderTexture(std::vector<GLuint> *m_indices, GLuint *m_VAO,
                     GLuint diffuseTexture, int numTriangles = -1) const;

  void loadDiffuseTexture(std::string_view path, GLuint *diffuseTexture);

  void setupVAO(GLuint program, GLuint *m_VBO, GLuint *m_EBO, GLuint *m_VAO);

  void destroy(GLuint *m_VBO, GLuint *m_EBO, GLuint *m_VAO) const;

private:
  void createBuffers(std::vector<Vertex> *m_vertices,
                     std::vector<GLuint> *m_indices, GLuint *m_VBO,
                     GLuint *m_EBO);

  void standardize(std::vector<Vertex> *m_vertices);

  void computeNormals(std::vector<Vertex> *m_vertices,
                      std::vector<GLuint> *m_indices);
};

#endif