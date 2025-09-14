#include "testBatching.h"

#include "TestBatching.h"
#include "Renderer/Renderer.h"
#include "vendor/imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
    TestBatching::TestBatching(GLFWwindow* window)
        : m_window(window),
        m_CameraPos(0.0f, 0.0f, 5.0f),
        m_CameraFront(0.0f, 0.0f, -1.0f),
        m_CameraUp(0.0f, 1.0f, 0.0f),
        m_CameraSpeed(2.5f),
        m_GridSize(10),
        m_Spacing(1.5f)
    {
        // Generate a batch of quads arranged in a grid
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        GenerateBatchData(vertices, indices);

        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3); // Positions (x,y,z)
        layout.Push<float>(3); // Colors   (r,g,b)
        m_VAO->AddBuffer(*m_VBO, layout);

        m_IBO = std::make_unique<IndexBuffer>(indices.data(), (unsigned int)indices.size());

        m_Shader = std::make_unique<Shader>("res/Shaders/BatchShader.shader");
        m_Shader->Bind();

        glEnable(GL_DEPTH_TEST);

        // Unbind everything to clean state
        m_VAO->unBind();
        m_VBO->Unbind();
        m_IBO->Unbind();
        m_Shader->Unbind();
    }

    void TestBatching::GenerateBatchData(std::vector<float>& vertices, std::vector<unsigned int>& indices) {
        // We will create a grid of quads. Each quad is made of two triangles.
        // Each quad: 4 vertices and 6 indices.
        // Vertex layout: pos(x,y,z), color(r,g,b)

        // Size of each quad
        float quadSize = 0.4f;

        // We’ll position quads in a grid, each quad spaced by m_Spacing.
        // Origin (0,0,0), with quads distributed around the origin.

        int quadCount = m_GridSize * m_GridSize;
        vertices.reserve(quadCount * 4 * 6); // each vertex: 6 floats (pos+color)
        indices.reserve(quadCount * 6);

        // Simple colour gradient
        auto vertexIndex = 0u;
        for (int y = 0; y < m_GridSize; y++) {
            for (int x = 0; x < m_GridSize; x++) {
                float offsetX = (x - m_GridSize / 2) * m_Spacing;
                float offsetY = (y - m_GridSize / 2) * m_Spacing;

                // Quad vertices: positions + colours
                // bottom-left
                vertices.push_back(offsetX - quadSize); vertices.push_back(offsetY - quadSize); vertices.push_back(0.0f);
                vertices.push_back((float)x / m_GridSize); vertices.push_back((float)y / m_GridSize); vertices.push_back(0.5f);

                // bottom-right
                vertices.push_back(offsetX + quadSize); vertices.push_back(offsetY - quadSize); vertices.push_back(0.0f);
                vertices.push_back((float)x / m_GridSize); vertices.push_back((float)y / m_GridSize); vertices.push_back(1.0f);

                // top-right
                vertices.push_back(offsetX + quadSize); vertices.push_back(offsetY + quadSize); vertices.push_back(0.0f);
                vertices.push_back((float)x / m_GridSize); vertices.push_back((float)y / m_GridSize); vertices.push_back(0.0f);

                // top-left
                vertices.push_back(offsetX - quadSize); vertices.push_back(offsetY + quadSize); vertices.push_back(0.0f);
                vertices.push_back((float)x / m_GridSize); vertices.push_back((float)y / m_GridSize); vertices.push_back(0.7f);

                // Indices for two triangles
                // Using the vertexIndex as the base for this quad
                indices.push_back(vertexIndex + 0);
                indices.push_back(vertexIndex + 1);
                indices.push_back(vertexIndex + 2);
                indices.push_back(vertexIndex + 2);
                indices.push_back(vertexIndex + 3);
                indices.push_back(vertexIndex + 0);

                vertexIndex += 4;
            }
        }
    }

    void TestBatching::ProcessInput() {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            m_CameraPos += m_CameraSpeed * m_CameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            m_CameraPos -= m_CameraSpeed * m_CameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed;
    }

    void TestBatching::Update(float deltaTime) {
        ProcessInput();
    }

    void TestBatching::Render() {
        Renderer renderer;
        renderer.ClearColour_White();
        renderer.Clear();

        m_Shader->Bind();
        m_VAO->Bind();
        m_IBO->Bind();

        // Model matrix (no rotation for simplicity)
        glm::mat4 model = glm::mat4(1.0f);

        // View and projection (similar to TestCamera)
        glm::mat4 view = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(15.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_Shader->setUniformMat4f("model", model);
        m_Shader->setUniformMat4f("view", view);
        m_Shader->setUniformMat4f("projection", projection);

        // Draw all quads in one call
        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }

    void TestBatching::RenderGUI() {
        ImGui::Text("Batching Demo");
        ImGui::SliderInt("Grid Size", &m_GridSize, 1, 50);
        ImGui::SliderFloat("Spacing", &m_Spacing, 0.5f, 5.0f);
        ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 0.1f, 10.0f);
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", m_CameraPos.x, m_CameraPos.y, m_CameraPos.z);
        if (ImGui::Button("Regenerate")) {
            std::vector<float> vertices;
            std::vector<unsigned int> indices;
            GenerateBatchData(vertices, indices);
            m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
            VertexBufferLayout layout;
            layout.Push<float>(3); // Positions (x,y,z)
            layout.Push<float>(3); // Colors   (r,g,b)
            m_VAO->AddBuffer(*m_VBO, layout);
            m_IBO = std::make_unique<IndexBuffer>(indices.data(), (unsigned int)indices.size());

        }
    }
}
