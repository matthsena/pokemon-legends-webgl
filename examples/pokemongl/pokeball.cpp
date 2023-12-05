#include "pokeball.hpp"
#include <glm/fwd.hpp>

void Pokeball::create(Model m_model, const std::string assetsPath)
{
    m_pokemon_program = abcg::createOpenGLProgram(
        {{.source = assetsPath + "shaders/texture.vert", .stage = abcg::ShaderStage::Vertex},
         {.source = assetsPath + "shaders/texture.frag",
          .stage = abcg::ShaderStage::Fragment}});

    // Carregamos os índices e vértices para a bola a partir do sphere.obj
    m_model.loadObj(assetsPath + "pokeball.obj", &m_vertices, &m_indices, &m_VBO,
                    &m_EBO);

    // Inicializamos os buffers para a parede
    m_model.setupVAO(m_pokemon_program, &m_VBO, &m_EBO, &m_VAO);

    m_viewMatrixLocation =
        abcg::glGetUniformLocation(m_pokemon_program, "viewMatrix");
    m_projMatrixLocation =
        abcg::glGetUniformLocation(m_pokemon_program, "projMatrix");

    m_colorLocation = abcg::glGetUniformLocation(m_pokemon_program, "color");

    m_model.loadDiffuseTexture(assetsPath + "pokeball.png", &m_diffuse_texture);

    float min_height = m_vertices[0].position.y;
    float max_height = m_vertices[0].position.y;

    for (const auto &vertex : m_vertices)
    {
        min_height = std::min(min_height, vertex.position.y);
        max_height = std::max(max_height, vertex.position.y);
    }

    m_pokeball_radius = ((max_height - min_height) / 2.0f) * SCALE;
}

void Pokeball::destroy()
{
    abcg::glDeleteProgram(m_pokemon_program);
    abcg::glDeleteBuffers(1, &m_EBO);
    abcg::glDeleteBuffers(1, &m_VBO);
    abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Pokeball::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model, glm::vec3 position)
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
    if (m_pokeballLaunched == true)
    {
        model = glm::translate(model, m_position);
    }
    else
    {
        model = glm::translate(model, position);
    }

    // m_captured deixa delay de metade do tempo
    if (m_captured == true)
    {
        frameTimer += 1;

        if (frameTimer > g.CATCH_FRAME_TIME * 2.0f)
        {
            fmt::print("[POKEBALL] Pokemon capturado!\n");
            m_pokeballLaunched = false;
            frameTimer = 0;
            m_captured = false;
        }
    }

    model = glm::scale(model, glm::vec3(SCALE));
    // deixando a pokebola virada para o lado que vai ser lancada
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));

    auto modelViewMatrix{glm::mat3(viewMatrix * model)};
    auto normalMatrix{glm::inverseTranspose(modelViewMatrix)};
    abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE,
                             &normalMatrix[0][0]);

    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
    abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

    m_model.renderTexture(&m_indices, &m_VAO, m_diffuse_texture);
    abcg::glUseProgram(0);
}