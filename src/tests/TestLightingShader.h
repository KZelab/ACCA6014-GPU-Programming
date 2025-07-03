#pragma once
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "Tests.h"
#include "../utils/Camera.h"
#include <memory>
#include "../Shader.h"
#include "glm/glm.hpp"
#include "../Mesh/Sphere.h"
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
		Camera camera;
		//lighting paramaters
		float m_AmbientIntensity;
		float m_DiffuseIntensity;
		float m_SpecularIntensity;
		float m_Shininess;
		//Light source properties
		glm::vec3 m_LightPositon;
		glm::vec3 m_LightColour;


		//Transformations
		glm::mat4 m_Model;
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		std::unique_ptr<Sphere> sphere;

		std::unique_ptr<Shader> m_PhongsShader;
		std::unique_ptr<Shader> m_FlatShader;
		std::unique_ptr<Shader> m_Gouraud;
		std::unique_ptr<Shader> m_BlinnPhong;

		int m_CurrentShader; //0 Phong, 1 flat, 2 Gouraud, 3 Blinn-Phong
	};
}

