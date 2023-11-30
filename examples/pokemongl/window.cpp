#include "window.hpp"

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE) {
      launchPokeball();
    }

    if (event.key.keysym.sym == SDLK_b) {
      m_showPokedex = !m_showPokedex;
    }

    if (event.key.keysym.sym == SDLK_r) {
      // std::thread restartGameThread(&Window::restartGame, this);
      // restartGameThread.detach();
      restartGame();
    }

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
  if (event.type == SDL_KEYUP) {
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

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 30.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "lookat.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "lookat.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_model, assetsPath);

  // Get location of uniform variables
  m_viewMatrixLocation = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLocation = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLocation = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_program, "color");

  for (size_t i = 0; i < m_modelPaths.size(); i++) {
    auto color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    std::string name = "";

    if (m_modelPaths[i] == "charmander.obj") {
      color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
      name = "Charmander";
    } else if (m_modelPaths[i] == "bulbasaur.obj") {
      color = glm::vec4(0.2f, 0.6f, 0.3f, 1.0f);
      name = "Bulbasaur";
    }

    auto const [vertices_pokemon, indices_pokemon] =
        loadModelFromFile(assetsPath + m_modelPaths[i]);

    GLuint tmp_VAO{};
    GLuint tmp_VBO{};
    GLuint tmp_EBO{};

    // Generate VBO
    abcg::glGenBuffers(1, &tmp_VBO);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, tmp_VBO);
    abcg::glBufferData(GL_ARRAY_BUFFER,
                       sizeof(vertices_pokemon.at(0)) * vertices_pokemon.size(),
                       vertices_pokemon.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate EBO
    abcg::glGenBuffers(1, &tmp_EBO);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tmp_EBO);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       sizeof(indices_pokemon.at(0)) * indices_pokemon.size(),
                       indices_pokemon.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Create VAO
    abcg::glGenVertexArrays(1, &tmp_VAO);

    // Bind vertex attributes to current VAO
    abcg::glBindVertexArray(tmp_VAO);

    abcg::glBindBuffer(GL_ARRAY_BUFFER, tmp_VBO);
    auto const positionAttribute{
        abcg::glGetAttribLocation(m_program, "inPosition")};
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tmp_EBO);

    // End of binding to current VAO
    abcg::glBindVertexArray(0);

    m_pokemons_list[m_modelPaths[i]] =
        Pokemon{tmp_VAO,         tmp_VBO, tmp_EBO, vertices_pokemon,
                indices_pokemon, color,   name};
  }

  // build pokeball
  auto const [vertices_pokeball, indices_pokeball] =
      loadModelFromFile(assetsPath + "pokeball.obj");
  m_vertices_pokeball = vertices_pokeball;
  m_indices_pokeball = indices_pokeball;

  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO_pokeball);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO_pokeball);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices_pokeball.at(0)) *
                         m_vertices_pokeball.size(),
                     m_vertices_pokeball.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO_pokeball);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_pokeball);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices_pokeball.at(0)) *
                         m_indices_pokeball.size(),
                     m_indices_pokeball.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO_pokeball);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO_pokeball);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO_pokeball);
  auto const positionAttribute_pokeball{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute_pokeball);
  abcg::glVertexAttribPointer(positionAttribute_pokeball, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_pokeball);

  abcg::glBindVertexArray(0);

  // Definindo posição inicial dos pokemons
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  std::uniform_real_distribution<float> rd_poke_position(-5.0f, 5.0f);
  std::uniform_int_distribution<int> rd_poke_model(0, m_modelPaths.size() - 1);

  // inicializando pokemons
  for (int i = 0; i < m_num_pokemons; ++i) {
    m_pokemon[i] = m_pokemons_list[m_modelPaths[rd_poke_model(m_randomEngine)]];
    m_pokemon[i].m_position = glm::vec3(rd_poke_position(m_randomEngine), 0,
                                        rd_poke_position(m_randomEngine));
  }
}

// https://stackoverflow.com/questions/321068/returning-multiple-values-from-a-c-function
std::tuple<std::vector<Vertex>, std::vector<GLuint>>
Window::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attributes{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  vertices.clear();
  indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      auto const vx{attributes.vertices.at(startIndex + 0)};
      auto const vy{attributes.vertices.at(startIndex + 1)};
      auto const vz{attributes.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = {vx, vy, vz}};

      // If map doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = vertices.size();
        // Add this vertex
        vertices.push_back(vertex);
      }

      indices.push_back(hash[vertex]);
    }
  }

  return std::make_tuple(vertices, indices);
}

void Window::onPaint() {
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

  // renderizando cada pokemon
  for (int i = 0; i < m_num_pokemons; ++i) {
    auto selectedPokemon = m_pokemon[i];

    abcg::glBindVertexArray(selectedPokemon.m_vao);

    glm::mat4 model{1.0f};
    // renderizacao condicional caso nao tenha sido capturado
    if (selectedPokemon.m_captured == false) {
      model = glm::translate(model, selectedPokemon.m_position);
      model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
      model = glm::scale(model, glm::vec3(0.02f));

      abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
                               &model[0][0]);
      abcg::glUniform4f(m_colorLocation, selectedPokemon.m_color.r,
                        selectedPokemon.m_color.g, selectedPokemon.m_color.b,
                        selectedPokemon.m_color.a);
      abcg::glDrawElements(GL_TRIANGLES, selectedPokemon.m_indices.size(),
                           GL_UNSIGNED_INT, nullptr);
    }
  }

  // DRAW Pokeball
  if (m_pokeballLaunched == true) {

    abcg::glBindVertexArray(m_VAO_pokeball);

    // model = glm::mat4(1.0);
    glm::mat4 model_pokeball{1.0f};
    model_pokeball = glm::translate(model_pokeball, m_pokeballPosition);
    model_pokeball =
        glm::rotate(model_pokeball, glm::radians(90.0f), glm::vec3(0, 1, 0));
    model_pokeball = glm::scale(model_pokeball, glm::vec3(0.002f));

    abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE,
                             &model_pokeball[0][0]);
    abcg::glUniform4f(m_colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
    abcg::glDrawElements(GL_TRIANGLES, m_indices_pokeball.size(),
                         GL_UNSIGNED_INT, nullptr);
  }

  abcg::glBindVertexArray(0);

  // Draw ground
  m_ground.paint(m_camera.getViewMatrix(), m_camera.getProjMatrix(), m_model);

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
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

    // https://stackoverflow.com/questions/64653747/how-to-center-align-text-horizontally
    if (m_currentState == PokemonState::Captured) {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f) {
        backToLive();
      }

      text = "Capturado!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());

    } else if (m_currentState == PokemonState::Escaped) {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f) {
        backToLive();
      }

      text = "Escapou!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }

    if (m_restarted == true) {
      frameTimer += 1;
      // ou seja, passou 1.5 segundo (90 frames)
      if (frameTimer > 90.0f) {
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
    if (m_showPokedex) {
      ImGui::Begin("Pokédex", nullptr, ImGuiWindowFlags_NoFocusOnAppearing);
      ImGui::Text("Pokémons capturados:");

      for (const auto &pokemon : m_pokedex_pokemons) {
        // ImGui::Text(pokemon.c_str());
        ImGui::TextUnformatted(pokemon.c_str());
      }

      // Adicione mais informações se necessário
      ImGui::End();
    }
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);

  // Atualiza a posição da Pokébola
  updatePokeballPosition();
  glm::vec3 sunPosition{-1.0f, 1.5f, -6.5f};
  glm::vec4 sunColor{1.0f, 1.0f, 0.0f, 1.0f};

  m_ground.update(sunColor, sunPosition);
}

void Window::launchPokeball() {
  if (!m_pokeballLaunched) {
    m_currentState = PokemonState::Live;

    fmt::print("Pokebola vai!\n");

    m_pokeballPosition = m_camera.getEyePosition();

    glm::vec3 launchDirection =
        glm::normalize(m_camera.getLookAtPoint() - m_camera.getEyePosition());
    float launchSpeed = 2.0f;
    m_pokeballVelocity = launchDirection * launchSpeed;
    m_pokeballLaunched = true;
  }
}

void Window::updatePokeballPosition() {
  if (m_pokeballLaunched) {
    auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

    const float pokeballRadius = 0.1f;
    const float pokemonRadius = 0.5f;

    m_pokeballPosition += m_pokeballVelocity * deltaTime;

    // Verifica se saiu da tela
    if ((m_pokeballPosition.x - pokeballRadius) < -5.0f ||
        (m_pokeballPosition.x - pokeballRadius) > 5.0f ||
        (m_pokeballPosition.z - pokeballRadius) < -5.0f ||
        (m_pokeballPosition.z - pokeballRadius) > 5.0f) {
      m_pokeballLaunched = false;
      fmt::print("Pokebola parou!\n");
    }

    // Verifica se colidiu com algum pokemon
    for (int i = 0; i < m_num_pokemons; ++i) {
      if (!m_pokemon[i].m_captured) {
        float distance =
            glm::distance(m_pokeballPosition, m_pokemon[i].m_position);

        if ((distance - pokemonRadius - pokeballRadius) < 0.02f) {
          // Colisão detectada
          fmt::print("Pokébola colidiu com Pokémon {}!\n", i + 1);
          // probabilidade de captura 45%
          std::uniform_real_distribution<float> rd_poke_capture(0.0f, 1.0f);

          if (rd_poke_capture(m_randomEngine) < 0.45f) {
            m_pokemon[i].m_captured = true;
            m_pokedex_pokemons.insert(m_pokemon[i].m_name);

            m_currentState = PokemonState::Captured;
          } else {
            m_currentState = PokemonState::Escaped;
          }

          m_pokeballLaunched = false;
          break;
        }
      }
    }
  }
}

void Window::backToLive() {
  m_currentState = PokemonState::Live;
  frameTimer = 0.0f;
}

void Window::restartGame() {
  std::uniform_real_distribution<float> rd_poke_position(-5.0f, 5.0f);

  for (int i = 0; i < m_num_pokemons; ++i) {
    m_pokemon[i].m_captured = false;
    m_pokemon[i].m_position = glm::vec3(rd_poke_position(m_randomEngine), 0,
                                        rd_poke_position(m_randomEngine));
  }
  m_pokeballLaunched = false;
  m_pokedex_pokemons.clear();

  m_showPokedex = false;

  m_restarted = true;
  frameTimer = 0.0f;
}