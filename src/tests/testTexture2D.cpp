#include "testTexture2D.h"


#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices

#include "vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements
#include "vendor/imgui/imgui_impl_glfw.h" // ImGui GLFW backend
#include "vendor/imgui/imgui_impl_opengl3.h" // ImGui OpenGL backend

#include <memory>
namespace test
{
	testTexture2D::testTexture2D() :m_TranslationA(200,200,0), m_TranslationB(400,400,0)
	{
        GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)); // Set up blending for transparency
        GlCall(glEnable(GL_BLEND));

        float positions[] = {
		   -50.0f, -50.0f, 0.0f, 0.0f, // Bottom-left
		    50.0f, -50.0f, 1.0f, 0.0f, // Bottom-right
		    50.0f,  50.0f, 1.0f, 1.0f, // Top-right
		   -50.0f,  50.0f, 0.0f, 1.0f  // Top-left
        };

        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        m_Shader = std::make_unique<Shader>(R"(res/Shaders/Basic.shader)");
        m_VAO = std::make_unique<VertexArray>();

        // Vertex Buffer Object (VBO) to manage the vertices in GPU memory
        m_VBO = std::make_unique<VertexBuffer>(positions, 4 * 4 * sizeof(float));

        // Layout definition for the Vertex Buffer (VBO)
        VertexBufferLayout layout;
        layout.Push<float>(2); // Position attributes
        layout.Push<float>(2); // Texture coordinates
        m_VAO->AddBuffer(*m_VBO, layout); // Add layout to VAO

        m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 6);

        // Projection matrix for 2D orthographic projection
        m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.f, -1.0f, 1.0f);
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));


        // Texture loading and binding
        m_Texture = std::make_unique<Texture>(R"(res/Textures/1.png)");

        m_Texture->Bind(); // Activate texture unit 0
        m_Shader->Bind(); // Make shader active so all shader-related operations refer to it
        m_Shader->setUniform1i("u_Texture", 0); // Link shader uniform to texture unit 0

        // Unbind objects to prevent accidental modifications or conflicts
        m_VAO->unBind();     // Unbind Vertex Array Object to prevent accidental reuse
        m_Shader->Unbind(); // Unbind Shader Program to ensure clean state
        m_VBO->Unbind();     // Unbind Vertex Buffer
        m_IndexBuffer->Unbind();     // Unbind Index Buffer
	}


	void testTexture2D::Update(float deltaTime)
	{
		
	}

	void testTexture2D::Render()
	{
        Renderer renderer;
        renderer.ClearColour_White();

        renderer.Clear();

        m_Texture->Bind();

        { // Rendering block for Object A
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA); // Model matrix for A
            glm::mat4 mvp = m_Proj * m_View * model; // Compute final MVP matrix
            m_Shader->Bind(); // Bind shader program for A, makes the shader program active so that subsequent drawing operations use this shader’s instructions.
            m_Shader->setUniformMat4f("u_MVP", mvp); // Send MVP matrix to shader
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader); // Draw object A
        }

        { // Rendering block for Object A
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB); // Model matrix for A
            glm::mat4 mvp = m_Proj * m_View * model; // Compute final MVP matrix
            m_Shader->Bind(); // Bind shader program for A, makes the shader program active so that subsequent drawing operations use this shader’s instructions.
            m_Shader->setUniformMat4f("u_MVP", mvp); // Send MVP matrix to shader
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader); // Draw object A
        }

	}

	void testTexture2D::RenderGUI()
	{
        // ImGui sliders to control object positions
        ImGui::SliderFloat3("Translation A", &m_TranslationA.x, 50.0f, 960.0f); // Adjust translation for A
        ImGui::SliderFloat3("Translation B", &m_TranslationB.x, 50.0f, 960.0f); // Adjust translation for B

        // Display framerate information
        float rate = ImGui::GetIO().Framerate;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / rate, rate);
	}
}

