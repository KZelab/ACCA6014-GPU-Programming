#include "DefaultScene.h"
#include "../Mesh/GeometryFactory.h"
#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
    DefaultScene::DefaultScene()
    {
        m_Shader = std::make_unique<Shader>("res/Shaders/DefaultScene.shader");

        // Flat cube slab — same technique as TestShadowMapping's ground plane.
        // Position (0, -0.05, 0) + Y scale 0.1 puts the top surface exactly at Y=0.
        m_FloorMesh = GeometryFactory::CreateCube();
        m_FloorMesh->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
        m_FloorMesh->setScale(glm::vec3(200.0f, 0.1f, 200.0f));
    }

    DefaultScene::~DefaultScene() = default;

    void DefaultScene::Render(const glm::mat4& view,
                              const glm::mat4& projection,
                              float tileSize)
    {
        glClearColor(0.53f, 0.71f, 0.90f, 1.0f);  // soft sky blue
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_Shader->Bind();
        m_Shader->setUniformMat4f("u_Model",      m_FloorMesh->getTransformMatrix());
        m_Shader->setUniformMat4f("u_View",       view);
        m_Shader->setUniformMat4f("u_Projection", projection);

        glm::vec3 lightDir = glm::normalize(glm::vec3(0.60f, 1.00f, 0.40f));
        m_Shader->setUniform3f("u_LightDir",  lightDir.x, lightDir.y, lightDir.z);
        m_Shader->setUniform1f("u_TileSize",  tileSize);

        m_FloorMesh->Draw();
    }
}
