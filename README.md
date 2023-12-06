# COMPUTAÇÃO GRÁFICA - Aplicação Interativa 3D

## Integrantes 

**Nome:** Gabriel Fernandes &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; **RA:** 11201720718
<br/>
**Nome:** Matheus Alexandre de Sena    &nbsp; &nbsp;  **RA:** 11201720166

## Link para WebAssembly

https://matthsena.github.io/pokemon-legends-webgl/pokemon-gl/

## Resumo da aplicação

O projeto `pokemon-legends-webgl` é uma evolução do segundo projeto `pokemon-gl` e também teve como inspiração o jogo Pokémon GO, muito jogado desde seu lançamento em 2016.
Neste projeto, o usuário da aplicação está em um cenário em primeira pessoa, onde ele procura Pokémons pelo espaço e faz o lançamento de pokebolas sob eles, podendo fazer a captura das espécies. 
Como evolução, nesta versão trouxemos um cenário com iluminação e textura, adicionando melhorias na jogabilidade fazendo a implementação de um menu Help, mira para a Pokebola na captura de Pokemon e gravidade no cenário onde a Pokebola é arremessada. Também foram realizadas melhorias quanto o objeto da Pokebola e a animação pós captura de um Pokémon.

![Alt text](image.png)

Para este projeto, foi utilizado a biblioteca `ABCg` (https://github.com/hbatagelo/abcg) disponibilizada no curso de Computação Gráfica 2023.3 na Universidade Federal do ABC.  

## Comandos

ESPAÇO: Dispara a Pokébola

**R**: Reinicia o jogo

**B**: Abre o menu Pokédex, onde você pode ver os Pokémons capturados

**A / Seta esquerda**: Movimenta para a esquerda

**D / Seta para direita**: Movimenta para a direita

**W / Seta para cima**: Movimenta para frente

**S / Seta para baixo**: Movimenta para trás

**I**: Movimenta a mira para cima

**J**: Movimenta a mira para baixo

**H**: Abre o menu "Help"


## Visão geral da implementação:

* window.hpp:

A classe Window é definida e herda da classe abcg::OpenGLWindow, que é uma parte da biblioteca abcgOpenGL para criar janelas gráficas.

### Variáveis importantes:

`struct Pokemon`: Variável struct que armazena os VBOs, EBOs, nome e outras características de cada Pokémon. Dessa forma, não temos uma variável global para essas definições, mas uma para cada obj.

```c++
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
```


`m_pokemons_list`: Hashmap que guarda os dados de VAO, VBO, EBO, Vertices e Indices de cada Pokémon existente na lista de arquivos `.obj`.
```c++
std::unordered_map<std::string, Pokemon> m_pokemons_list;
```

`m_modelPaths`: Variável que armazena uma lista dos arquivos .obj que podem ser renderizados na aplicação de forma aleatória.
```c++
  std::vector<std::string> m_modelPaths = {"charmander.obj", "bulbasaur.obj"};
```

`m_miraPosition`: Variável que inicializa a mira na posição correta {0, 0}.

`GRAVITY`: Variável que define a gravidade sob o lançamento da pokebola.

`m_font`: Variável utilizada para a renderização dos textos que são apresentados na tela (Escapou!, Capturado!, Jogo Reiniciado)


## window.cpp:

O arquivo window.cpp é composto pelas funções utilizadas para a construção lógica da aplicação renderizada. Abaixo segue um resumo descritivo de cada função presente no arquivo:

`onEvent`: Função que manipula os eventos SDL, como pressionar as teclas do teclado. 
I e K: Move a mira para cima e para baixo. Também foi adicionado um max e min para restringir a mira dentro da tela
Espaço: Lança a pokébola através da chamada de `launchPokeball`
R: Reinicia o jogo através da chamada de `restartGameThread` 
B: Abre o Pokédex com a listagem de Pokémons capturados a partir de `m_showPokedex`

Também foram definidas as setas e as teclas AWSD para o comando de movimentação do usuário em primeira pessoa.

`onCreate`: Função chamada para inicializar a aplicação. Os shaders são chamados nos arquivos `lookat.frag` e `lookat.vert`. Além disso, no onCreate é aplicada a configuração do nome e as cores dos Pokémons, conforme o trecho de código abaixo:

```c++
  for (int i = 0; i < 2; i++) {
    auto color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    std::string name = "";

    if (m_modelPaths[i] == "charmander.obj") {
      color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
      name = "Charmander";
    } else if (m_modelPaths[i] == "Bulbasaur.obj") {
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
...
  
  }
```

Os VBOs e EBOs para cada Pokémon é definido separadamente através da variável Pokemon apresentada anteriormente na seção do window.hpp. Desta forma, definidos assim dentro do onCreate:

```c++
...
    m_pokemons_list[m_modelPaths[i]] =
        Pokemon{tmp_VAO,         tmp_VBO, tmp_EBO, vertices_pokemon,
                indices_pokemon, color,   name};
  }
```


A posição e o tipo de Pokémon que sera renderizado é construído pela lógica abaixo, onde as duas definições são feitas de forma aleatória pela função `rd_poke_position` e `rd_poke_model`, respectivamente:

```c++
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

```

`loadModelFromFile`: Função de carregamento dos arquivos .obj. O retorno dela é uma tupla com os vertices e indices das posições do objeto (Pokémon ou Pokébola):

```c++
std::tuple<std::vector<Vertex>, std::vector<GLuint>>
Window::loadModelFromFile(std::string_view path) {

  .
  .
  .

  return std::make_tuple(vertices, indices);
}
```


`onPaint`: Função que renderiza a cena, utilizando shaders para renderizar os Pokémons, a Pokébola e o chão. A renderização de cada Pokémon acontece conforme o código abaixo:

```c++
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
```

`onPaintUI`: Define uma interface de usuário (UI) usando a biblioteca ImGui. 

A mira é renderizada através da definição de variáveis como `miraRadius` que define o raio da mira e `miraColor`que define a cor da mira. Abaixo temos o código onde na primeira etapa as variáveis são definidas e na segunda etapa o desenho da mira é renderizado:

```c++
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
```

A ImGui é utizada no `onPaintUI` para exibir as frases na tela durante a execução da aplicação, conforme o código abaixo:

```c++
    if (m_currentState == PokemonState::Captured) {
      text = "Capturado!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());

    } else if (m_currentState == PokemonState::Escaped) {
      text = "Escapou!";
      textWidth = ImGui::CalcTextSize(text.c_str()).x;
      ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
      ImGui::TextUnformatted(text.c_str());
    }

    if (m_restarted == true) {
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
    }
```

`onResize`: É chamado quando a janela é redimensionada e atualiza o tamanho do viewport da câmera.

`onDestroy`: É chamado quando a janela é destruída e é usado para limpar os recursos OpenGL.

`onUpdate`: É chamado para atualizar a lógica do jogo a cada quadro, como a movimentação da câmera.

`launchPokeball`: Chamada quando o jogador pressiona ESPAÇO para fazer o lançamento da Pokébola.

`updatePokeballPosition`: Atualiza a posição da Pokébola durante o movimento.

`backToLive`: Utilizada para atualizar as informações do jogo em tempo de execução.

`restartGame`: Chamada para reiniciar o jogo.

## main.cpp:

O arquivo main.cpp inicia a aplicação, criando uma instância da classe Window e realizando as configurações da janela de exibição (width, height, title). Em seguida, inicia a aplicação com `app.run(window)`.

## ground.cpp:

O arquivo ground.cpp é composto pelas funções utilizadas na classe Ground, que são utilizadas para criar o VAO e VBO do chão.

## ground.hpp: 

O arquivo ground.hpp define a classe Ground que é utilizada para criação e renderização do chão.

## camera.cpp: 

O arquivo camera.cpp é composto pelas funções utilizadas na classe Camera para calcular as matrizes de projeção e visualização, bem como manipular a posição e orientação da câmera.

## camera.hpp:

O arquivo camera.hpp define a classe Camera que é usada para a visualização da cena pelo usuário.

Foram definidos 2 getters para capturar e retornar a posição da camêra (m_eye) e o ponto para onde a câmera está olhando (m_at), conforme abaixo:

```c++
// ADICIONADO - GETTERS DE POSICAO DA CAMERA
  glm::vec3 const getEyePosition() const { return m_eye; }
  glm::vec3 const getLookAtPoint() const { return m_at; }
```

Eles são utilizados na função de lançamento da pokébola `launchPokeball` para realizar os cálculos de posição da pokébola e posição de lançamento, conforme o código abaixo do arquivo `window.cpp`:

```c++
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
```
