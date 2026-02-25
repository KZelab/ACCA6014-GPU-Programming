#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../Framebuffer.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test
{
	class TestShadowMapping : public Tests
	{
	public:
		TestShadowMapping(GLFWwindow* window);
		~TestShadowMapping();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;

	private:
		void RenderScene(Shader& shader);
		void RecreateShadowMap();

		GLFWwindow* m_Window;

		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Shader> m_DepthShader;
		std::unique_ptr<Shader> m_PhongShader;
		std::unique_ptr<Framebuffer> m_ShadowFBO;

		// Scene objects
		std::unique_ptr<Mesh> m_GroundPlane;
		std::unique_ptr<Mesh> m_Cubes[3];
		std::unique_ptr<Mesh> m_Spheres[2];

		// Transforms
		glm::mat4 m_View;
		glm::mat4 m_Projection;
		glm::mat4 m_LightSpaceMatrix;

		// Light
		glm::vec3 m_LightDirection;
		glm::vec3 m_LightColour;

		// Shadow params
		float m_ShadowBias;
		int m_ShadowResolution;
		bool m_EnablePCF;
		int m_PCFKernelSize;
		bool m_ShowDebugShadowMap;

		// Phong params
		float m_AmbientIntensity;
		float m_DiffuseIntensity;
		float m_SpecularIntensity;
		float m_Shininess;
		glm::vec3 m_ObjectColor;

		// Light ortho params
		float m_OrthoSize;
		float m_NearPlane;
		float m_FarPlane;
	};
}
