#include "TestMesh.h"
#include "../vendor/imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace test
{
	TestMesh::TestMesh(GLFWwindow* window)
		: m_Window(window), m_ShowCube(true), m_ShowCustomMesh(true), m_Wireframe(false),
		  m_MeshType(0), m_Model1(1.0f), m_Model2(1.0f), m_RotationSpeed(1.0f), m_Time(0.0f)
	{
		// Initialize renderer
		m_Renderer = std::make_unique<Renderer>();

		// Setup projection and view matrices
		int width, height;
		glfwGetFramebufferSize(m_Window, &width, &height);
		m_Projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		m_View = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		// Create cube mesh using factory method - demonstrates RAII and factory pattern
		m_CubeMesh = std::unique_ptr<Mesh>(Mesh::CreateCube());

		// Create initial custom mesh (triangle)
		CreateCustomTriangle();

		// Create basic shader for rendering
		std::string vertexShader = R"(
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

		std::string fragmentShader = R"(
			#version 330 core
			in vec3 vertexColour;
			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(vertexColour, 1.0);
			}
		)";

		m_BasicShader = std::make_unique<Shader>(vertexShader, fragmentShader);
	}

	TestMesh::~TestMesh()
	{
	}

	void TestMesh::CreateCustomTriangle()
	{
		// Demonstrate custom mesh creation with structured vertex data
		std::vector<Vertex> vertices = {
			// Position             Colour              UV
			{{0.0f,  0.8f, 0.0f},  {1.0f, 0.0f, 0.0f}, {0.5f, 1.0f}}, // Top - red
			{{-0.8f, -0.8f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom left - green
			{{0.8f, -0.8f, 0.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}  // Bottom right - blue
		};

		std::vector<unsigned int> indices = {
			0, 1, 2  // Single triangle
		};

		m_CustomMesh = std::make_unique<Mesh>(vertices, indices);
	}

	void TestMesh::CreateCustomQuad()
	{
		// Demonstrate custom mesh creation with quad geometry
		std::vector<Vertex> vertices = {
			// Position             Colour              UV
			{{-0.8f, -0.8f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom left - red
			{{0.8f, -0.8f, 0.0f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom right - green
			{{0.8f,  0.8f, 0.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top right - blue
			{{-0.8f,  0.8f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}  // Top left - yellow
		};

		std::vector<unsigned int> indices = {
			0, 1, 2,  // First triangle
			2, 3, 0   // Second triangle
		};

		m_CustomMesh = std::make_unique<Mesh>(vertices, indices);
	}

	void TestMesh::Update(float deltaTime)
	{
		m_Time += deltaTime;

		// Update model matrices for rotation
		m_Model1 = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
		m_Model1 = glm::rotate(m_Model1, m_Time * m_RotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));

		m_Model2 = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
		m_Model2 = glm::rotate(m_Model2, m_Time * m_RotationSpeed * 0.7f, glm::vec3(1.0f, 0.0f, 1.0f));

		// Update custom mesh based on selection
		static int lastMeshType = -1;
		if (m_MeshType != lastMeshType) {
			switch (m_MeshType) {
				case 1: CreateCustomTriangle(); break;
				case 2: CreateCustomQuad(); break;
			}
			lastMeshType = m_MeshType;
		}
	}

	void TestMesh::Render()
	{
		m_Renderer->Clear();

		// Set wireframe mode if requested
		if (m_Wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (m_BasicShader) {
			m_BasicShader->Bind();

			// Render cube mesh (factory-created)
			if (m_ShowCube && m_CubeMesh) {
				glm::mat4 mvp = m_Projection * m_View * m_Model1;
				m_BasicShader->setUniformMat4f("u_MVP", mvp);

				m_CubeMesh->Bind();
				glDrawElements(GL_TRIANGLES, m_CubeMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
			}

			// Render custom mesh (manually created)
			if (m_ShowCustomMesh && m_CustomMesh) {
				glm::mat4 mvp = m_Projection * m_View * m_Model2;
				m_BasicShader->setUniformMat4f("u_MVP", mvp);

				m_CustomMesh->Bind();
				glDrawElements(GL_TRIANGLES, m_CustomMesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
			}
		}

		// Reset polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void TestMesh::RenderGUI()
	{
		ImGui::Text("Mesh Class Demonstration");
		ImGui::Text("This test demonstrates the abstracted Mesh class capabilities");
		ImGui::Separator();

		// Mesh visibility controls
		ImGui::Text("Mesh Visibility:");
		ImGui::Checkbox("Show Cube (Factory)", &m_ShowCube);
		ImGui::Checkbox("Show Custom Mesh", &m_ShowCustomMesh);
		ImGui::Checkbox("Wireframe Mode", &m_Wireframe);

		ImGui::Separator();

		// Custom mesh type selection
		ImGui::Text("Custom Mesh Type:");
		ImGui::RadioButton("Triangle", &m_MeshType, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Quad", &m_MeshType, 2);

		ImGui::Separator();

		// Animation controls
		ImGui::Text("Animation:");
		ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 3.0f);

		ImGui::Separator();

		// Mesh information
		ImGui::Text("Mesh Information:");
		if (m_CubeMesh) {
			ImGui::Text("Cube: %d indices", m_CubeMesh->GetIndexCount());
		}
		if (m_CustomMesh) {
			ImGui::Text("Custom: %d indices", m_CustomMesh->GetIndexCount());
		}

		ImGui::Separator();

		// Feature descriptions
		ImGui::Text("Features Demonstrated:");
		ImGui::BulletText("Factory method pattern (Mesh::CreateCube)");
		ImGui::BulletText("Custom mesh creation with structured vertices");
		ImGui::BulletText("RAII resource management (VAO/VBO/EBO)");
		ImGui::BulletText("Vertex data encapsulation");
		ImGui::BulletText("Multiple mesh rendering");
		ImGui::BulletText("Dynamic mesh recreation");
	}
}