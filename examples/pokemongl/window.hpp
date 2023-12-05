#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"
#include "pokemon.hpp"
#include "pokeball.hpp"

#include <chrono>
#include <random>
// #include <thread>
#include <tuple>
#include <unordered_map>
#include <set>

class Window : public abcg::OpenGLWindow
{
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  GLuint m_tutorialTexture;
  bool m_showTutorial{true};

  std::vector<Pokemon> pokemons_spawned; // pokemons que já foram spawnados

  std::vector<std::string> m_modelPaths = {"pokemons/Charmander.obj", "pokemons/Bulbasaur.obj"};

  int m_num_pokemons{5};
  // MIRA
  glm::vec2 m_miraPosition{0, 0};
  // // GRAVIDADE
  const float GRAVITY = -0.98f;

  std::set<std::string> m_pokedex_pokemons;

  bool m_showPokedex{false};
  bool m_restarted{false};

  glm::ivec2 m_viewportSize{};

  ImFont *m_font{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

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

  Model m_model;

  Ground m_ground;

  Pokemon m_pokemon_render;

  Pokeball m_pokeball_render;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  std::vector<Vertex> m_vertices_pokeball;
  std::vector<GLuint> m_indices_pokeball;

  std::tuple<std::vector<Vertex>, std::vector<GLuint>>
  loadModelFromFile(std::string_view path);

  // Estados da Pokébola
  glm::vec3 m_pokeballPosition{};
  glm::vec3 m_pokeballVelocity{};
  bool m_pokeballLaunched{false};

  enum class PokemonState
  {
    Captured,
    Escaped,
    Live
  };
  PokemonState m_currentState{PokemonState::Live};

  void launchPokeball();
  void updatePokeballPosition();
  // void checkPokemonCapture();

  void backToLive();
  void restartGame();

  std::tuple<std::vector<Vertex>, std::vector<GLuint>> createSphere(float radius, unsigned int sectors, unsigned int stacks);

  int frameTimer{0};
};

#endif