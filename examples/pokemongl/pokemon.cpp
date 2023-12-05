#include "pokemon.hpp"
#include <glm/fwd.hpp>

void Pokemon::create(Model m_model, const std::string assetsPath, std::string objPath, glm::vec3 position)
{
    m_pokemon_program = abcg::createOpenGLProgram(
        {{.source = assetsPath + "shaders/texture.vert", .stage = abcg::ShaderStage::Vertex},
         {.source = assetsPath + "shaders/texture.frag",
          .stage = abcg::ShaderStage::Fragment}});

    // Carregamos os índices e vértices para a bola a partir do sphere.obj
    m_model.loadObj(assetsPath + objPath, &m_vertices, &m_indices, &m_VBO,
                    &m_EBO);

    // Inicializamos os buffers para a parede
    m_model.setupVAO(m_pokemon_program, &m_VBO, &m_EBO, &m_VAO);

    m_viewMatrixLocation =
        abcg::glGetUniformLocation(m_pokemon_program, "viewMatrix");
    m_projMatrixLocation =
        abcg::glGetUniformLocation(m_pokemon_program, "projMatrix");

    m_colorLocation = abcg::glGetUniformLocation(m_pokemon_program, "color");

    m_model.loadDiffuseTexture(assetsPath + objPath + ".png", &m_diffuse_texture);
    m_model.loadDiffuseTexture(assetsPath + "captured.png", &m_captured_texture);

    // Pega o tamanho do modelo para deixar y rente ao chao
    float min_height = m_vertices[0].position.y;
    float max_height = m_vertices[0].position.y;

    float min_width = m_vertices[0].position.x;
    float max_width = m_vertices[0].position.x;

    for (const auto &vertex : m_vertices)
    {
        min_height = std::min(min_height, vertex.position.y);
        max_height = std::max(max_height, vertex.position.y);

        min_width = std::min(min_width, vertex.position.x);
        max_width = std::max(max_width, vertex.position.x);
    }

    y = -min_height;

    m_position = glm::vec3(position.x, -min_height, position.z);
    setPokemonName(objPath);

    m_pokemon_radius = ((max_width - min_width) / 2.0f);
    m_pokemon_width = max_width - min_width;
    m_pokemon_height = max_height - min_height;

}

void Pokemon::destroy()
{
    abcg::glDeleteProgram(m_pokemon_program);
    abcg::glDeleteBuffers(1, &m_EBO);
    abcg::glDeleteBuffers(1, &m_VBO);
    abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Pokemon::setPokemonName(std::string name)
{
    // regex para remover ".obj" do nome
    std::regex e(".obj");
    m_pokemonName = std::regex_replace(name, e, "");
}

bool Pokemon::getPokemonCaptured()
{
    return m_captured;
}

void Pokemon::setPokemonCaptured(bool captured)
{
    m_captured = captured;
}

void Pokemon::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model)
{

    abcg::glUseProgram(m_pokemon_program);

    // Localização das matrizes
    auto const viewMatrixLoc{
        abcg::glGetUniformLocation(m_pokemon_program, "viewMatrix")};
    auto const projMatrixLoc{
        abcg::glGetUniformLocation(m_pokemon_program, "projMatrix")};
    auto const modelMatrixLoc{
        abcg::glGetUniformLocation(m_pokemon_program, "modelMatrix")};
    auto const normalMatrixLoc{
        abcg::glGetUniformLocation(m_pokemon_program, "normalMatrix")};

    auto const lightLoc{abcg::glGetUniformLocation(m_pokemon_program, "lightPos")};

    // Localização das propriedades de iluminação do sol
    auto const shininessLoc{
        abcg::glGetUniformLocation(m_pokemon_program, "shininess")};
    auto const IaLoc{abcg::glGetUniformLocation(m_pokemon_program, "Ia")};
    auto const IdLoc{abcg::glGetUniformLocation(m_pokemon_program, "Id")};
    auto const IsLoc{abcg::glGetUniformLocation(m_pokemon_program, "Is")};
    auto const KaLoc{abcg::glGetUniformLocation(m_pokemon_program, "Ka")};
    auto const KdLoc{abcg::glGetUniformLocation(m_pokemon_program, "Kd")};
    auto const KsLoc{abcg::glGetUniformLocation(m_pokemon_program, "Ks")};

    // Bind das propriedades
    abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
    abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projMatrix[0][0]);

    // Propriedades da luz
    abcg::glUniform4fv(lightLoc, 1, &lightPos.x);
    abcg::glUniform4fv(IaLoc, 1, &Ia.x);
    abcg::glUniform4fv(IdLoc, 1, &Id.x);
    abcg::glUniform4fv(IsLoc, 1, &Is.x);
    abcg::glUniform4fv(KaLoc, 1, &Ka.x);
    abcg::glUniform4fv(KdLoc, 1, &Kd.x);
    abcg::glUniform4fv(KsLoc, 1, &Ks.x);
    abcg::glUniform1f(shininessLoc, shininess);

    // float height = maxHeight - minHeight;

    glm::mat4 model{1.0f};
    // Ajuste a posição do modelo no eixo Y para que ele esteja rente ao chão
    model = glm::translate(model, m_position);
    float newScale = 1.0f;

    if (m_captured)
    {
        frameTimer += 1;

        // 150 = 2.5s
        if (frameTimer > g.CATCH_FRAME_TIME)
        {
            destroy();
            m_captured = false;
            frameTimer = 0;
        }
        // diminui scale gradualmente
        newScale = 1.0f - (frameTimer / g.CATCH_FRAME_TIME);

        // ajusta a posicao em Y
        float heightDifference = (1.0f - newScale) * m_pokemon_height;
        m_position.y = y - heightDifference / 2.0f;
    }

    model = glm::scale(model, glm::vec3(newScale));
    // adiciona um contador para qnd a variavel m_captured for verdadeira ir diminuindo a escala do pokemon a cada segundo ate zero

    auto modelViewMatrix{glm::mat3(viewMatrix * model)};
    auto normalMatrix{glm::inverseTranspose(modelViewMatrix)};
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE,
                             &normalMatrix[0][0]);

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
    abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

    if (m_captured)
    {
        m_model.renderTexture(&m_indices, &m_VAO, m_captured_texture);
    }
    else
    {
        m_model.renderTexture(&m_indices, &m_VAO, m_diffuse_texture);
    }
    abcg::glUseProgram(0);
}