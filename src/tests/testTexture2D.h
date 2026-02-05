#pragma once
#include "Tests.h"
#include "../Renderer.h"               // Custom renderer wrapper

#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"
#include "../Texture.h"

#include "glm/glm.hpp"              // GLM for mathematical operations and matrix manipulation
#include "glm/gtc/matrix_transform.hpp" // For GLM transformations, like translating matrices

#include "../vendor/imgui/imgui.h"     // Dear ImGui library for GUI elements
#include "../vendor/imgui/imgui_impl_glfw.h" // ImGui GLFW backend
#include "../vendor/imgui/imgui_impl_opengl3.h" // ImGui OpenGL backend

#include <memory>

namespace test
{
	class testTexture2D :public Tests
	{
	public:
		testTexture2D();

		void Update(float deltaTime) override;
		void Render() override;
		void RenderGUI() override;
	private:
		// Define translation vectors for two objects
		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA, m_TranslationB;
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<VertexBuffer> m_VBO;
		std::unique_ptr<Texture> m_Texture;


	};
	
}
