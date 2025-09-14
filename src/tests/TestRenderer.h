#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Tests.h"
#include "../Renderer.h"
#include "../Shader.h"
#include "../Mesh.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
	class TestRenderer : public Tests
	{
	public:
		TestRenderer(GLFWwindow* window);
		~TestRenderer();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Renderer> m_Renderer;
		std::unique_ptr<Shader> m_BasicShader;
		std::unique_ptr<Mesh> m_CubeMesh;

		// Interactive parameters for testing renderer features
		glm::vec4 m_ClearColour;
		bool m_EnableDepthTest;
		bool m_EnableBlending;
		bool m_EnableCulling;
		bool m_WireframeMode;
		int m_CullFace; // 0=Back, 1=Front, 2=Both

		// Multiple objects for depth testing demonstration
		std::vector<glm::vec3> m_CubePositions;
		std::vector<glm::vec3> m_CubeColours;

		// Transformation matrices
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		float m_Time;
	};
}