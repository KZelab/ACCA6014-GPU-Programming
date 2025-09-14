#include "testProjections.h"
#include "Renderer/Renderer.h"
#include "vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements
#include <GL/glew.h>     
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices
namespace test
{
    TestProjections::TestProjections() 
    {

        useOrthographic = true;
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

	}
	void TestProjections::Update(float deltaTime)
	{

	}

	void TestProjections::Render()
	{
        Renderer renderer;
        renderer.ClearColour_White();

        renderer.Clear();

        m_Shader->Bind();


        // Model matrix: rotating cube
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        m_Shader->setUniformMat4f("model", model);
        
        // View matrix: camera at (0, 0, 3)
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        m_Shader->setUniformMat4f("view", view);
       

        glm::mat4 projection;
        if (useOrthographic) {
            std::cout << "Orthographic mode \n";
            projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
        }
        else {
            std::cout << "perspective mode \n";
            projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        }
        m_VAO->Bind();
        m_Shader->setUniformMat4f("projection", projection);
        renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader); // Draw object A


	}

	void TestProjections::RenderGUI()
	{
        ImGui::Text("Currently using: %s mode", useOrthographic ? "Orthographic" : "Perspective");

        if(!useOrthographic)
	        if (ImGui::Button("Orthographic mode"))
	        {
	            useOrthographic = true;
	        }
        if(useOrthographic)
	        if (ImGui::Button("Perspective mode"))
	        {
	            useOrthographic = false;
	        }        
	}



}
