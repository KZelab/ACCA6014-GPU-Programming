#include "TestShader.h"
#include "../vendor/imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace test
{
	TestShader::TestShader(GLFWwindow* window)
		: m_Window(window), m_CubeColour(1.0f, 0.5f, 0.2f), m_TimeScale(1.0f),
		  m_WaveAmplitude(0.1f), m_WaveFrequency(2.0f), m_CurrentShader(0),
		  m_Model(1.0f), m_Time(0.0f)
	{
		// Initialize renderer
		m_Renderer = std::make_unique<Renderer>();

		// Create cube mesh using our abstracted factory
		m_CubeMesh = std::unique_ptr<Mesh>(Mesh::CreateCube());

		// Setup projection and view matrices
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		m_Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		m_View = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Basic shader - demonstrates simple uniform handling
		std::string basicVertex = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColour;
			layout (location = 2) in vec2 aTexCoord;

			uniform mat4 u_MVP;
			out vec3 vertexColour;

			void main()
			{
				gl_Position = u_MVP * vec4(aPos, 1.0);
				vertexColour = aColour;
			}
		)";

		std::string basicFragment = R"(
			#version 330 core
			in vec3 vertexColour;
			out vec4 FragColor;

			uniform vec3 u_Colour;

			void main()
			{
				FragColor = vec4(u_Colour * vertexColour, 1.0);
			}
		)";

		// Colour mixing shader - demonstrates multiple uniform types
		std::string colourVertex = basicVertex;
		std::string colourFragment = R"(
			#version 330 core
			in vec3 vertexColour;
			out vec4 FragColor;

			uniform vec3 u_Colour;
			uniform float u_MixFactor;

			void main()
			{
				vec3 finalColour = mix(vertexColour, u_Colour, u_MixFactor);
				FragColor = vec4(finalColour, 1.0);
			}
		)";

		// Animated shader - demonstrates time-based uniforms
		std::string animatedVertex = R"(
			#version 330 core
			layout (location = 0) in vec3 aPos;
			layout (location = 1) in vec3 aColour;
			layout (location = 2) in vec2 aTexCoord;

			uniform mat4 u_MVP;
			uniform float u_Time;
			uniform float u_WaveAmplitude;
			uniform float u_WaveFrequency;

			out vec3 vertexColour;

			void main()
			{
				vec3 pos = aPos;
				pos.y += sin(u_Time * u_WaveFrequency + pos.x * 3.14159) * u_WaveAmplitude;

				gl_Position = u_MVP * vec4(pos, 1.0);
				vertexColour = aColour;
			}
		)";

		std::string animatedFragment = R"(
			#version 330 core
			in vec3 vertexColour;
			out vec4 FragColor;

			uniform float u_Time;
			uniform vec3 u_Colour;

			void main()
			{
				float pulse = (sin(u_Time * 2.0) + 1.0) * 0.5;
				vec3 finalColour = mix(vertexColour, u_Colour, pulse);
				FragColor = vec4(finalColour, 1.0);
			}
		)";

		// Create shader objects using our abstracted Shader class
		m_BasicShader = std::make_unique<Shader>(basicVertex, basicFragment);
		m_ColourShader = std::make_unique<Shader>(colourVertex, colourFragment);
		m_AnimatedShader = std::make_unique<Shader>(animatedVertex, animatedFragment);
	}

	TestShader::~TestShader()
	{
	}

	void TestShader::Update(float deltaTime)
	{
		m_Time += deltaTime * m_TimeScale;

		// Update model matrix for rotation
		m_Model = glm::rotate(glm::mat4(1.0f), m_Time, glm::vec3(1.0f, 0.3f, 0.5f));
	}

	void TestShader::Render()
	{
		m_Renderer->Clear();

		// Select current shader based on user choice
		Shader* currentShader = nullptr;
		switch (m_CurrentShader) {
			case 0: currentShader = m_BasicShader.get(); break;
			case 1: currentShader = m_ColourShader.get(); break;
			case 2: currentShader = m_AnimatedShader.get(); break;
		}

		if (currentShader && m_CubeMesh) {
			currentShader->Bind();

			// Set common uniforms
			glm::mat4 mvp = m_Projection * m_View * m_Model;
			currentShader->setUniformMat4f("u_MVP", mvp);
			currentShader->setUniform3f("u_Colour", m_CubeColour.x, m_CubeColour.y, m_CubeColour.z);

			// Set shader-specific uniforms
			if (m_CurrentShader == 1) { // Colour mixing shader
				static float mixFactor = 0.5f;
				mixFactor = (sin(m_Time) + 1.0f) * 0.5f; // Animate mix factor
				currentShader->setUniform1f("u_MixFactor", mixFactor);
			}
			else if (m_CurrentShader == 2) { // Animated shader
				currentShader->setUniform1f("u_Time", m_Time);
				currentShader->setUniform1f("u_WaveAmplitude", m_WaveAmplitude);
				currentShader->setUniform1f("u_WaveFrequency", m_WaveFrequency);
			}

			// Bind mesh and draw using our abstracted classes
			m_CubeMesh->Bind();
			glDrawElements(GL_TRIANGLES, m_CubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
		}
	}

	void TestShader::RenderGUI()
	{
		ImGui::Text("Shader Class Demonstration");
		ImGui::Text("This test demonstrates the abstracted Shader class capabilities");
		ImGui::Separator();

		// Shader selection
		ImGui::Text("Shader Selection:");
		ImGui::RadioButton("Basic Shader", &m_CurrentShader, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Colour Mixing", &m_CurrentShader, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Animated", &m_CurrentShader, 2);

		ImGui::Separator();

		// Common parameters
		ImGui::Text("Shader Uniforms:");
		ImGui::ColorEdit3("Cube Colour", &m_CubeColour.x);
		ImGui::SliderFloat("Time Scale", &m_TimeScale, 0.0f, 3.0f);

		// Shader-specific parameters
		if (m_CurrentShader == 2) { // Animated shader
			ImGui::Text("Animation Parameters:");
			ImGui::SliderFloat("Wave Amplitude", &m_WaveAmplitude, 0.0f, 0.5f);
			ImGui::SliderFloat("Wave Frequency", &m_WaveFrequency, 0.5f, 5.0f);
		}

		ImGui::Separator();

		// Feature descriptions
		ImGui::Text("Features Demonstrated:");
		ImGui::BulletText("Shader compilation and linking");
		ImGui::BulletText("Uniform variable setting (float, vec3, mat4)");
		ImGui::BulletText("Multiple shader programs");
		ImGui::BulletText("Real-time uniform updates");
		ImGui::BulletText("Integration with Mesh and Renderer classes");
	}
}