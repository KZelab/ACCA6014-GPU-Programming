#pragma once
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "Tests.h"
#include "../Mesh/FullscreenQuad.h"
#include "../Texture.h"


namespace test
{
	class testEffects : public Tests
	{
	public:
		testEffects(GLFWwindow* window);

		void Update(float deltaTime);
		void Render();
		void RenderGUI();

	private:
		GLFWwindow* m_Window;

		std::unique_ptr<FullscreenQuad> m_Quad;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Shader> m_Shader;


		int m_Effect = 0; //selected effect
		float m_Opacity = 1.0f; 

	};
}

