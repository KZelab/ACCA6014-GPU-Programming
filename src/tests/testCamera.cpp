#include "testCamera.h"
#include "Renderer/Renderer.h"
#include "vendor/imgui/imgui.h"
#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"

namespace test {
    TestCamera::TestCamera(GLFWwindow* window)
        :
        m_window(window),
		cameraPosition(0.0f, 0.0f, 3.0f),
        cameraFront(0.0f, 0.0f, -1.0f),
        cameraUp(0.0f, 1.0f, 0.0f),
        cameraSpeed(2.5f)
    {
        // Vertex data for a simple cube
        const float vertices[] = {
            // Positions           // Colors
            -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // Bottom-left
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f, // Bottom-right
             0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f, // Top-right
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, // Top-left
            -0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 1.0f, // Bottom-left (front)
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f, // Bottom-right (front)
             0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f, // Top-right (front)
            -0.5f,  0.5f,  0.5f,   0.5f, 0.5f, 0.5f  // Top-left (front)
        };

        // Cube indices for drawing with Element Buffer
        const unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            4, 5, 1, 1, 0, 4,
            7, 6, 2, 2, 3, 7,
            4, 7, 3, 3, 0, 4,
            1, 5, 6, 6, 2, 1
        };

        m_Shader = std::make_unique<Shader>(R"(res/Shaders/ProjectionsShader.shader)");

        m_VAO = std::make_unique<VertexArray>();

        m_VBO = std::make_unique<VertexBuffer>(vertices, 48 * sizeof(float));// Vertex Buffer Object (VBO) to manage the vertices in GPU memory


        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        m_VAO->AddBuffer(*m_VBO, layout);


        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 36);

        // Enable depth testing
        glEnable(GL_DEPTH_TEST);

        // Unbind objects to prevent accidental modifications or conflicts
        m_VAO->unBind();     // Unbind Vertex Array Object to prevent accidental reuse
        m_Shader->Unbind(); // Unbind Shader Program to ensure clean state
        m_VBO->Unbind();     // Unbind Vertex Buffer
        m_IndexBuffer->Unbind();     // Unbind Index Buffer
        // Initialize vertex and index data (similar to your cube in TestProjections)
        // Shader, VAO, VBO, IndexBuffer setup, depth test enabling
    }

    void TestCamera::Update(float deltaTime) {
        ProcessInput();
    }

    void TestCamera::Render() {
        Renderer renderer;
        renderer.ClearColour_White();
        renderer.Clear();

        m_Shader->Bind();

        // Set the model matrix
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        m_Shader->setUniformMat4f("model", model);

        // Set the view matrix based on camera position and orientation
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        m_Shader->setUniformMat4f("view", view);

        // Set the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        m_Shader->setUniformMat4f("projection", projection);

        m_VAO->Bind();
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
    }

    void TestCamera::ProcessInput() {
        // Assuming we have access to the GLFW window object
        GLFWwindow* window = glfwGetCurrentContext();  // This should be correctly initialized in your application.

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
    }

    void TestCamera::RenderGUI() {
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 10.0f);
    }
}
