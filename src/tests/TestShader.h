#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh.h"
#include "../Renderer.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace test
{
	class TestShader : public Tests
	{
	public:
		TestShader(GLFWwindow* window);
		~TestShader();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Shader> m_BasicShader;
		std::unique_ptr<Shader> m_ColourShader;
		std::unique_ptr<Shader> m_AnimatedShader;
		std::unique_ptr<Mesh> m_CubeMesh;
		std::unique_ptr<Renderer> m_Renderer;

		// Interactive parameters for testing shader features
		glm::vec3 m_CubeColour;
		float m_TimeScale;
		float m_WaveAmplitude;
		float m_WaveFrequency;
		int m_CurrentShader; // 0=Basic, 1=Colour, 2=Animated

		// Transformation matrices
		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		float m_Time;
	};
}