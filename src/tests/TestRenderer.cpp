#include "TestRenderer.h"
#include "../vendor/imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace test
{
	TestRenderer::TestRenderer(GLFWwindow* window)
		: m_Window(window), m_ClearColour(0.2f, 0.3f, 0.8f, 1.0f),
		  m_EnableDepthTest(true), m_EnableBlending(false), m_EnableCulling(true),
		  m_WireframeMode(false), m_CullFace(0), m_Time(0.0f)
	{
		// Initialize our abstracted renderer
		m_Renderer = std::make_unique<Renderer>();

		// Setup projection and view matrices
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		m_Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		m_View = glm::lookAt(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Create cube mesh
		m_CubeMesh = std::unique_ptr<Mesh>(Mesh::CreateCube());

		// Setup multiple cubes at different depths for depth testing demonstration
		m_CubePositions = {
			glm::vec3( 0.0f,  0.0f,  0.0f),   // Center
			glm::vec3( 2.0f,  1.0f, -2.0f),   // Back right
			glm::vec3(-2.0f, -1.0f, -1.0f),   // Front left
			glm::vec3( 1.0f, -2.0f,  1.0f),   // Very front
			glm::vec3(-1.0f,  2.0f, -3.0f)    // Very back
		};

		m_CubeColours = {
			glm::vec3(1.0f, 0.0f, 0.0f),   // Red
			glm::vec3(0.0f, 1.0f, 0.0f),   // Green
			glm::vec3(0.0f, 0.0f, 1.0f),   // Blue
			glm::vec3(1.0f, 1.0f, 0.0f),   // Yellow
			glm::vec3(1.0f, 0.0f, 1.0f)    // Magenta
		};

		// Create shader for rendering
		std::string vertexShader = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColour;
			layout (location = 2) in vec2 aTexCoord;

			uniform mat4 u_MVP;
			uniform vec3 u_ObjectColour;
			out vec3 vertexColour;

			void main()
			{
				gl_Position = u_MVP * vec4(aPos, 1.0);
				vertexColour = aColour * u_ObjectColour;
			}
		)";

		std::string fragmentShader = R"(
			#version 330 core
			in vec3 vertexColour;
			out vec4 FragColor;

			uniform float u_Alpha;

			void main()
			{
				FragColor = vec4(vertexColour, u_Alpha);
			}
		)";

		m_BasicShader = std::make_unique<Shader>(vertexShader, fragmentShader);

		// Initialize renderer state
		m_Renderer->EnableDepthTesting();
		m_Renderer->EnableBackfaceCulling();
		m_Renderer->SetClearColour(m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, m_ClearColour.a);
	}

	TestRenderer::~TestRenderer()
	{
	}

	void TestRenderer::Update(float deltaTime)
	{
		m_Time += deltaTime;

		// Update renderer state based on GUI controls
		m_Renderer->SetClearColour(m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, m_ClearColour.a);

		// Toggle depth testing
		if (m_EnableDepthTest) {
			m_Renderer->EnableDepthTesting();
		} else {
			m_Renderer->DisableDepthTesting();
		}

		// Toggle blending
		if (m_EnableBlending) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		} else {
			glDisable(GL_BLEND);
		}

		// Toggle face culling
		if (m_EnableCulling) {
			glEnable(GL_CULL_FACE);
			switch (m_CullFace) {
				case 0: glCullFace(GL_BACK); break;
				case 1: glCullFace(GL_FRONT); break;
				case 2: glCullFace(GL_FRONT_AND_BACK); break;
			}
		} else {
			glDisable(GL_CULL_FACE);
		}

		// Toggle wireframe mode
		if (m_WireframeMode) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	void TestRenderer::Render()
	{
		// Use our abstracted renderer's clear method
		m_Renderer->Clear();

		if (m_BasicShader && m_CubeMesh) {
			m_BasicShader->Bind();

			// Render multiple cubes to demonstrate depth testing and state management
			for (size_t i = 0; i < m_CubePositions.size(); ++i) {
				glm::mat4 model = glm::translate(glm::mat4(1.0f), m_CubePositions[i]);
				model = glm::rotate(model, m_Time + i * 0.5f, glm::vec3(1.0f, 0.3f, 0.5f));

				glm::mat4 mvp = m_Projection * m_View * model;
				m_BasicShader->setUniformMat4f("u_MVP", mvp);
				m_BasicShader->setUniform3f("u_ObjectColour",
					m_CubeColours[i].x, m_CubeColours[i].y, m_CubeColours[i].z);

				// Set alpha for blending demonstration
				float alpha = m_EnableBlending ? 0.7f : 1.0f;
				m_BasicShader->setUniform1f("u_Alpha", alpha);

				// Use our abstracted mesh binding
				m_CubeMesh->Bind();
				glDrawElements(GL_TRIANGLES, m_CubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
			}
		}

		// Reset polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void TestRenderer::RenderGUI()
	{
		ImGui::Text("Renderer Class Demonstration");
		ImGui::Text("This test demonstrates the abstracted Renderer class capabilities");
		ImGui::Separator();

		// Clear colour control
		ImGui::Text("Clear Settings:");
		ImGui::ColorEdit4("Clear Colour", &m_ClearColour.x);

		ImGui::Separator();

		// OpenGL state controls
		ImGui::Text("OpenGL State Management:");
		ImGui::Checkbox("Enable Depth Testing", &m_EnableDepthTest);
		ImGui::Checkbox("Enable Blending", &m_EnableBlending);
		ImGui::Checkbox("Enable Face Culling", &m_EnableCulling);
		ImGui::Checkbox("Wireframe Mode", &m_WireframeMode);

		if (m_EnableCulling) {
			ImGui::Text("Cull Face:");
			ImGui::RadioButton("Back", &m_CullFace, 0);
			ImGui::SameLine();
			ImGui::RadioButton("Front", &m_CullFace, 1);
			ImGui::SameLine();
			ImGui::RadioButton("Both", &m_CullFace, 2);
		}

		ImGui::Separator();

		// Information about the test
		ImGui::Text("Visual Effects:");
		ImGui::BulletText("5 cubes at different depths");
		ImGui::BulletText("Depth testing shows/hides overlap");
		ImGui::BulletText("Blending creates transparency");
		ImGui::BulletText("Face culling affects visibility");
		ImGui::BulletText("Wireframe shows geometry structure");

		ImGui::Separator();

		// Feature descriptions
		ImGui::Text("Features Demonstrated:");
		ImGui::BulletText("Centralised clear colour management");
		ImGui::BulletText("Depth buffer clearing and testing");
		ImGui::BulletText("OpenGL state abstraction");
		ImGui::BulletText("Error checking with GlCall macro");
		ImGui::BulletText("High-level rendering interface");
		ImGui::BulletText("Integration with Shader and Mesh classes");
	}
}