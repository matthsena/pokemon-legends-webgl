#ifndef POKEMON_HPP_
#define POKEMON_HPP_

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include <regex>

class Pokemon
{
public:
  void create(Model m_model, const std::string assetsPath, std::string objPath, glm::vec3 position);
  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model);
  void destroy();

  bool getPokemonCaptured();
  void setPokemonCaptured(bool captured);

  // Posição do pokemon
  glm::vec3 getPosition() const { return m_position; }
  void setPosition(glm::vec3 position) { m_position = position; }

  // Nome do pokemon
  std::string getPokemonName() const { return m_pokemonName; }
  void setPokemonName(std::string name);

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_pokemon_program{};

  GLuint m_diffuse_texture{};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_colorLocation{};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  // iluminação
  glm::vec4 lightPos{0.0f, 1.0f, 0.0f, 0.0f};
  glm::vec4 Ia{1.0f};
  glm::vec4 Ka{0.3f, 0.3f, 0.3f, 1.0f};
  glm::vec4 Id{1.0f};
  glm::vec4 Is{1.0f};
  glm::vec4 Kd{0.6f};
  glm::vec4 Ks{0.0f};
  float shininess{500.0f};
  // propriedades do pokemonm
  bool m_captured{false};
  glm::vec3 m_position;
  std::string m_pokemonName;
  int frameTimer{0};

  float h{0};
  float y{0};
};

#endif