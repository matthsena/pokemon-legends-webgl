#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"
#include <chrono>
#include <random>
// #include <thread>
#include <tuple>
#include <unordered_map>
#include <set>

struct Vertex {
  glm::vec3 position;

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  struct Pokemon {
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_ebo{};
    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    glm::vec4 m_color{};
    std::string m_name{};
    bool m_captured{false};
    glm::vec3 m_position{0, 0, 0};
  };


  std::unordered_map<std::string, Pokemon> m_pokemons_list;
  std::vector<std::string> m_modelPaths = {"charmander.obj", "bulbasaur.obj"};

  int m_num_pokemons{5};
  Pokemon m_pokemon[5];
  std::set<std::string> m_pokedex_pokemons;

  bool m_showPokedex{false};
  bool m_restarted{false};


  glm::ivec2 m_viewportSize{};

  ImFont *m_font{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  GLuint m_VAO_pokeball{};
  GLuint m_VBO_pokeball{};
  GLuint m_EBO_pokeball{};

  GLuint m_program{};

  std::default_random_engine m_randomEngine;

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  Camera m_camera;
  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};

  Ground m_ground;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  std::vector<Vertex> m_vertices_pokeball;
  std::vector<GLuint> m_indices_pokeball;

  // void loadModelFromFile(std::string_view path);

  std::tuple<std::vector<Vertex>, std::vector<GLuint>>
  loadModelFromFile(std::string_view path);

  // Estados da Pokébola
  glm::vec3 m_pokeballPosition{};
  glm::vec3 m_pokeballVelocity{};
  bool m_pokeballLaunched{false};

  enum class PokemonState { Captured, Escaped, Live };
  PokemonState m_currentState{PokemonState::Live};

  void launchPokeball();
  void updatePokeballPosition();
  // void checkPokemonCapture();

  void backToLive();
  void restartGame();

  int frameTimer{0};
};

#endif