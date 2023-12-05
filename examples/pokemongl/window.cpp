#include "window.hpp"

// Explicit specialization of std::hash for Vertex
template <>
struct std::hash<Vertex>
{
  size_t operator()(Vertex const &vertex) const noexcept
  {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event)
{
  if (m_showTutorial && event.key.keysym.sym != SDLK_h)
    return;

  if (event.key.keysym.sym == SDLK_i)
    m_miraPosition.y -= 10.0f; // Mover para cima

  if (event.key.keysym.sym == SDLK_k)
    m_miraPosition.y += 10.0f; // Mover para baixo

  // Restrição para manter a mira na tela
  m_miraPosition.y = std::max(m_miraPosition.y, 0.0f);
  m_miraPosition.y = std::min(m_miraPosition.y, static_cast<float>(m_viewportSize.y));

  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_SPACE)
      launchPokeball();

    if (event.key.keysym.sym == SDLK_b)
      m_showPokedex = !m_showPokedex;

    if (event.key.keysym.sym == SDLK_r)
      restartGame();

    if (event.key.keysym.sym == SDLK_h)
      m_showTutorial = !m_showTutorial;

    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_q)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_e)
      m_truckSpeed = 1.0f;
  }
  if (event.type == SDL_KEYUP)
  {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT ||
         event.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_q && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_e && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }
}

void Window::onCreate()
{
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  // carregar imagem
  abcg::glDeleteTextures(1, &m_tutorialTexture);
  m_tutorialTexture = abcg::loadOpenGLTexture({.path = assetsPath + "tutorial.png"});

  // Load a new font
  auto const filename{assetsPath + "fonts/Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 30.0f);
  if (m_font == nullptr)
  {
    throw abcg::RuntimeError("Cannot load font file");
  }

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "shaders/lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "shaders/lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_model, assetsPath);

  m_pokeball_render.create(m_model, assetsPath);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  // Definindo posição inicial dos pokemons
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_real_distribution<float> rd_poke_position(-5.0f, 5.0f);
  std::uniform_int_distribution<int> rd_poke_model(0, m_modelPaths.size() - 1);

  // inicializando pokemons
  for (int i = 0; i < m_num_pokemons; ++i)
  {
    std::string objFile = m_modelPaths[rd_poke_model(m_randomEngine)];
    glm::vec3 position = glm::vec3(rd_poke_position(m_randomEngine), 0,
                                   rd_poke_position(m_randomEngine));
    Pokemon pokemon;
    pokemon.create(m_model, assetsPath, objFile, position);

    pokemons_spawned.push_back(pokemon);
  }
}

void Window::onPaint()
{
  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  // interate pokemons_spawned
  for (auto &pokemon : pokemons_spawned)
  {
    // if (pokemon.getPokemonCaptured() == false)
    pokemon.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);
  }

  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);
  m_pokeball_render.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model, m_camera.getEyePosition());

  abcg::glUseProgram(0);
}

void Window::onPaintUI()
{
  abcg::OpenGLWindow::onPaintUI();
  {
    // TEXT WINDOW
    auto const size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    std::string text = "";
    float windowWidth = ImGui::GetWindowWidth();
    float textWidth = 0;

    // Desenhar a mira
    {
      float miraRadius = 10.0f;
      ImU32 miraColor = IM_COL32(255, 0, 0, 255);
      float lineThickness = 2.0f;
      ImVec2 center(m_viewportSize.x / 2.0f, m_miraPosition.y / 2.0f);

      // Ajustar o tamanho da janela para garantir que a mira caiba completamente
      ImVec2 windowSize = ImVec2(miraRadius * 4, miraRadius * 4);
      ImVec2 windowPos = ImVec2(center.x - windowSize.x / 2, center.y - windowSize.y / 2);

      ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
      ImGui::SetNextWindowSize(windowSize);
      ImGui::SetNextWindowBgAlpha(0);

      ImGui::Begin("MiraWindow", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav);
      ImDrawList *drawList = ImGui::GetWindowDrawList();

      drawList->AddCircle(center, miraRadius, miraColor, 0, lineThickness);

      ImGui::End();
    }

    if (m_showTutorial)
    {
      // Calcular o tamanho do quadrado com base na menor dimensão (altura normalmente)
      float squareSize = std::min(m_viewportSize.x, m_viewportSize.y) * 0.75f;

      // quadrado para centralizar
      float posX = (m_viewportSize.x - squareSize) / 2.0f;
      float posY = (m_viewportSize.y - squareSize) / 2.0f;

      // Desenhar o tutorial
      ImGui::SetNextWindowPos(ImVec2(posX, posY));
      ImGui::SetNextWindowSize(ImVec2(squareSize, squareSize));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0)); // Remover a borda da janela
      ImGui::Begin("Tutorial", nullptr, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav);

      ImGui::Image((void *)(intptr_t)m_tutorialTexture, ImVec2(squareSize, squareSize), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
      ImGui::End();
      ImGui::PopStyleVar(); // Restaurar o espaçamento da janela
    }

    // https://stackoverflow.com/questions/64653747/how-to-center-align-text-horizontally
    if (m_currentState == PokemonState::Captured)
    {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f)
      {
        backToLive();
      }

      text = "Capturado!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }
    else if (m_currentState == PokemonState::Escaped)
    {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f)
      {
        backToLive();
      }

      text = "Escapou!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }

    if (m_restarted == true)
    {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f)
      {
        m_restarted = false;
        frameTimer = 0.0f;
      }

      text = "Jogo reiniciado";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }

    ImGui::PopFont();
    ImGui::End();

    text = "";

    // JANELA DA POKEDEX
    if (m_showPokedex)
    {
      ImGui::Begin("Pokédex", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
      ImGui::Text("Pokémons capturados:");

      for (const auto &pokemon : m_pokedex_pokemons)
      {
        // ImGui::Text(pokemon.c_str());
        ImGui::TextUnformatted(pokemon.c_str());
      }

      // Adicione mais informações se necessário
      ImGui::End();
    }
  }
}

void Window::onResize(glm::ivec2 const &size)
{
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy()
{
  m_ground.destroy();
  m_pokeball_render.destroy();
  // destroy em cada pokemon spawnado

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  abcg::glDeleteTextures(1, &m_tutorialTexture);
}

void Window::onUpdate()
{
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);

  // Atualiza a posição da Pokébola
  updatePokeballPosition();
  glm::vec3 sunPosition{-1.0f, 2.5f, -6.5f};
  glm::vec4 sunColor{1.0f, 1.0f, 0.0f, 1.0f};

  m_ground.update(sunColor, sunPosition);
}

void Window::launchPokeball()
{
  if (!m_pokeball_render.getPokeballLaunched())
  {
    m_currentState = PokemonState::Live;
    fmt::print("Pokebola vai!\n");
    m_pokeballPosition = m_camera.getEyePosition();
    m_pokeball_render.setPosition(m_pokeballPosition);

    // Apenas uma declaração para cada variável
    float miraOffsetY = (m_miraPosition.y - (m_viewportSize.y / 2.0f)) / m_viewportSize.y;
    float launchSpeed = 2.0f;

    // Obter a direção da câmera
    glm::vec3 cameraDirection = glm::normalize(m_camera.getLookAtPoint() - m_camera.getEyePosition());

    // Ajustar com base na posição da mira
    glm::vec3 adjustedDirection = cameraDirection + glm::vec3(0, -miraOffsetY, 0);
    m_pokeballVelocity = glm::normalize(adjustedDirection) * launchSpeed;

    m_pokeball_render.setPokeballLaunched(true);
  }
}

void Window::updatePokeballPosition()
{
  if (m_pokeball_render.getPokeballLaunched())
  {
    auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

    // Atualize a componente vertical da velocidade devido à gravidade
    m_pokeballVelocity.y += GRAVITY * deltaTime;

    // Atualize a posição da pokebola
    m_pokeballPosition += m_pokeballVelocity * deltaTime;

    const float pokeballRadius = 0.1f;
    const float pokemonRadius = 0.5f;

    m_pokeballPosition += m_pokeballVelocity * deltaTime;

    m_pokeball_render.setPosition(m_pokeballPosition);

    // Verifica se saiu da tela
    if ((m_pokeballPosition.x - pokeballRadius) < -5.0f ||
        (m_pokeballPosition.x - pokeballRadius) > 5.0f ||
        (m_pokeballPosition.z - pokeballRadius) < -5.0f ||
        (m_pokeballPosition.z - pokeballRadius) > 5.0f)
    {
      m_pokeball_render.setPokeballLaunched(false);
      fmt::print("Pokebola parou!\n");

      m_pokeball_render.setPosition(m_camera.getEyePosition());
    }

    // Verifica se colidiu com algum pokemon
    for (auto &pokemon : pokemons_spawned)
    {
      if (pokemon.getPokemonCaptured() == false)
      {
        float distance = glm::distance(m_pokeballPosition, pokemon.getPosition());

        if ((distance - pokemonRadius - pokeballRadius) < 0.02f)
        {
          // Colisão detectada
          fmt::print("Pokébola colidiu com Pokémon!\n");
          m_pokeball_render.setPosition(m_camera.getEyePosition());

          // probabilidade de captura 45%
          std::uniform_real_distribution<float> rd_poke_capture(0.0f, 1.0f);

          if (rd_poke_capture(m_randomEngine) < 0.45f)
          {
            pokemon.setPokemonCaptured(true);
            m_pokedex_pokemons.insert(pokemon.getPokemonName());

            m_currentState = PokemonState::Captured;
          }
          else
          {
            m_currentState = PokemonState::Escaped;
          }

          m_pokeball_render.setPokeballLaunched(false);
          break;
        }
      }
    }
  }
}

void Window::backToLive()
{
  m_currentState = PokemonState::Live;
  frameTimer = 0.0f;
}

void Window::restartGame()
{
  std::uniform_real_distribution<float> rd_poke_position(-5.0f, 5.0f);

  for (auto &pokemon : pokemons_spawned)
  {
    pokemon.setPokemonCaptured(false);
    pokemon.setPosition(glm::vec3(rd_poke_position(m_randomEngine), pokemon.getPosition().y,
                                  rd_poke_position(m_randomEngine)));

    m_pokedex_pokemons.clear();

    m_showPokedex = false;

    m_restarted = true;
    frameTimer = 0.0f;
  }
}