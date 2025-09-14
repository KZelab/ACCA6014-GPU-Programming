#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Tests.h"
#include "../Mesh.h"
#include "../Shader.h"
#include "../Renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
	class TestMesh : public Tests
	{
	public:
		TestMesh(GLFWwindow* window);
		~TestMesh();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Mesh> m_CubeMesh;
		std::unique_ptr<Mesh> m_CustomMesh;
		std::unique_ptr<Shader> m_BasicShader;
		std::unique_ptr<Renderer> m_Renderer;

		// Interactive parameters for testing mesh features
		bool m_ShowCube;
		bool m_ShowCustomMesh;
		bool m_Wireframe;
		int m_MeshType; // 0=Cube, 1=Custom Triangle, 2=Custom Quad

		// Transformation matrices
		glm::mat4 m_Model1, m_Model2;
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		// Animation
		float m_RotationSpeed;
		float m_Time;

		// Custom mesh creation helpers
		void CreateCustomTriangle();
		void CreateCustomQuad();
	};
}