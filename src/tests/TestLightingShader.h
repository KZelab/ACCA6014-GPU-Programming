#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test
{
	class TestLightingShader : public Tests
	{
	public:
		TestLightingShader(GLFWwindow* window);

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
		GLFWwindow* m_Window;

		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Mesh> m_Sphere;
		std::unique_ptr<Shader> m_PhongShader;
		std::unique_ptr<Shader> m_FlatShader;
		std::unique_ptr<Shader> m_GouraudShader;
		std::unique_ptr<Shader> m_BlinnPhongShader;

		// Lighting parameters
		float m_AmbientIntensity;
		float m_DiffuseIntensity;
		float m_SpecularIntensity;
		float m_Shininess;

		// Light source properties
		glm::vec3 m_LightPosition;
		glm::vec3 m_LightColour;

		// Transformations
		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		int m_CurrentShader = 0; // 0: Phong, 1: Flat, 2: Gouraud, 3: Blinn-Phong
		bool m_Wireframe = false;
	};
}

