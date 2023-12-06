#include "ground.hpp"
#include <glm/fwd.hpp>

void Ground::create(Model m_model, const std::string assetsPath) {

  groundProgram = abcg::createOpenGLProgram(
      {{.source = assetsPath + "shaders/texture.vert", .stage = abcg::ShaderStage::Vertex},
       {.source = assetsPath + "shaders/texture.frag",
        .stage = abcg::ShaderStage::Fragment}});

  // Carregamos os índices e vértices para a bola a partir do sphere.obj
  m_model.loadObj(assetsPath + "ground.obj", &m_vertices, &m_indices, &m_VBO,
                  &m_EBO);

  // Inicializamos os buffers para a parede
  m_model.setupVAO(groundProgram, &m_VBO, &m_EBO, &m_VAO);

  groundViewMatrixLocation =
      abcg::glGetUniformLocation(groundProgram, "viewMatrix");
  groundProjMatrixLocation =
      abcg::glGetUniformLocation(groundProgram, "projMatrix");

  groundColorLocation = abcg::glGetUniformLocation(groundProgram, "color");

  m_model.loadDiffuseTexture(assetsPath + "ground.png", &diffuseTexture);
}

void Ground::update(glm::vec4 lightColorParam, glm::vec3 LightPosParam) {
  // Acertamos a luz especular, "brilho", com a cor da luz incidente
  lightPos = glm::vec4(LightPosParam, 0);
  shininess = abs(LightPosParam.x);
  Is = lightColorParam;
}

void Ground::destroy() {
  abcg::glDeleteProgram(groundProgram);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Ground::paint(glm::mat4 viewMatrix, glm::mat4 projMatrix, Model m_model) {

  abcg::glUseProgram(groundProgram);

  // Localização das matrizes
  auto const viewMatrixLoc{
      abcg::glGetUniformLocation(groundProgram, "viewMatrix")};
  auto const projMatrixLoc{
      abcg::glGetUniformLocation(groundProgram, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(groundProgram, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(groundProgram, "normalMatrix")};

  auto const lightLoc{abcg::glGetUniformLocation(groundProgram, "lightPos")};

  // Localização das propriedades de iluminação do sol
  auto const shininessLoc{
      abcg::glGetUniformLocation(groundProgram, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(groundProgram, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(groundProgram, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(groundProgram, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(groundProgram, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(groundProgram, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(groundProgram, "Ks")};

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

  // Criamos um tapete de placas de 0,5 por 0,5 (ground.obj) com as translações
  auto const N{10};
  for (auto const z : iter::range(-N, N + 1)) {
    for (auto const x : iter::range(-N - 3, N + 4)) {

      glm::mat4 model{1.0f};
      model = glm::translate(model, glm::vec3{x, 0.0f, z});

      auto modelViewMatrix{glm::mat3(viewMatrix * model)};
      auto normalMatrix{glm::inverseTranspose(modelViewMatrix)};
      abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE,
                               &normalMatrix[0][0]);

      abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
      abcg::glUniform4f(groundColorLocation, 1.0f, 0.8f, 0.0f, 1.0f);

      m_model.renderTexture(&m_indices, &m_VAO, diffuseTexture);
    }
  }
  abcg::glUseProgram(0);
}