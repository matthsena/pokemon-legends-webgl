#ifndef POKEBALL_HPP_
#define POKEBALL_HPP_

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include <regex>

class Pokeball
{
public:
  void create(Model m_model, const std::string assetsPath);
  void paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model, glm::vec3 position);
  void update(bool pokeballLaunched, glm::vec3 velocity);
  void destroy();

  bool getPokemonCaptured();
  void setPokemonCaptured(bool captured);

  void setPosition(glm::vec3 position) { m_position = position; }
  glm::vec3 getPosition() const { return m_position; }

private:
  bool m_pokeballLaunched{false};
  float width, height, depth{0};

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
  glm::vec3 m_position{0.0f, 0.0f, 0.0f};
};

#endif