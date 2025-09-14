#include "testShader.h"
#include "testShader.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"

test::testShader::testShader(GLFWwindow* window) :
    m_CameraPos(0.0f, 0.0f, 3.0f),
    m_Window(window) {

    // Load shaders and model
    //m_Shader = std::make_unique<Shader>("res/shaders/ShaderArt/plasma.shader");
    m_Shader = std::make_unique<Shader>("res/shaders/ShaderArt/noise.shader");
    m_Shader->Bind();

    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    m_Resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height)); // Convert to float if needed


    // Full-screen quad vertices and indices
    m_Vertices = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top-right
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Top-left
    };
    m_Indices = { 0, 1, 2, 2, 3, 0 };

    // Set up buffers
    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float));
    m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

    // Define vertex layout: position (3 floats) and texCoord (2 floats)
    VertexBufferLayout layout;
    layout.Push<float>(3); // Position
    layout.Push<float>(2); // TexCoord
    m_VAO->AddBuffer(*m_VBO, layout);

    m_VAO->unBind();
    m_VBO->Unbind();
    m_IBO->Unbind();
    m_Shader->Unbind();
}




void test::testShader::Update(float deltaTime) {

	ProcessInput(deltaTime);

}

void test::testShader::Render() {
    Renderer renderer;
    renderer.Clear();

    m_Shader->Bind();

    // Set uniform values
    float timeValue = static_cast<float>(glfwGetTime());//this block of code passes time as a uniform
    m_Shader->setUniform1f("u_Time", timeValue);
    m_Shader->setUniform2f("u_Resolution", m_Resolution.x, m_Resolution.y);
    m_Shader->setUniform3f("u_CameraPosition", m_CameraPos.x, m_CameraPos.y, m_CameraPos.z);

    renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
}

void test::testShader::RenderGUI() {
    ImGui::Text("Shader Testing GUI");

    //ImGui::SliderFloat("Time Multiplier", &m_Time, 1.0f, 100.0f);
    ImGui::Text("Resolution: %.1fx%.1f", m_Resolution.x, m_Resolution.y);
    //ImGui::SliderFloat2("Resolution", glm::value_ptr(m_Resolution), 100.0f, 1920.0f);

}

void test::testShader::ProcessInput(float deltaTime)
{

}