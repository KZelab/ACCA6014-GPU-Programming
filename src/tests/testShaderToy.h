#pragma once
#include <memory>

#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "Tests.h"
#include "MeshGenerator/FullscreenQuad.h"


namespace test {
	class testShaderToy : public Tests
	{
	public:
		testShaderToy(GLFWwindow* window);
		void Update(float deltaTime);
		void Render();
		void RenderGUI();
	private:
        GLFWwindow* m_window;




        std::unique_ptr<FullscreenQuad> quad;
		std::unique_ptr<Texture> m_Texture;



        std::unique_ptr<Shader> m_Shader;
		int m_Effect = 0; // Selected effect (0 = None, 1 = Edge Detection, etc.)
		float m_Opacity = 1.0f; // Default fully visible effect 
	};
}
