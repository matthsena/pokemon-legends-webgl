#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"
#include "pokemon.hpp"
#include "pokeball.hpp"
#include "globals.hpp"

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
  Globals g;

  GLuint m_tutorialTexture;
  bool m_showTutorial{true};

  std::vector<Pokemon> pokemons_spawned; // pokemons que já foram spawnados

  std::vector<std::string> m_modelPaths = {
      "pokemons/Blastoise.obj",
      "pokemons/Charmander.obj",
      "pokemons/Ivysaur.obj",
      "pokemons/VenusaurM.obj",
      "pokemons/Bulbasaur.obj",
      "pokemons/Charmeleon.obj",
      "pokemons/Squirtle.obj",
      "pokemons/Charizard.obj",
      "pokemons/Wartortle.obj",
      "pokemons/PikachuM.obj"};

  int m_num_pokemons{50};
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

  Pokeball m_pokeball_render;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  // Estados da Pokébola
  glm::vec3 m_pokeballPosition{};
  glm::vec3 m_pokeballVelocity{};

  enum class PokemonState
  {
    Captured,
    Escaped,
    Live
  };
  PokemonState m_currentState{PokemonState::Live};

  void launchPokeball();
  void updatePokeballPosition();

  void backToLive();
  void restartGame();

  int frameTimer{0};
};

#endif