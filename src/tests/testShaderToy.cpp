#include "testShaderToy.h"

#include "imgui/imgui.h"

test::testShaderToy::testShaderToy(GLFWwindow* window) : m_window(window)
{
	m_Shader = std::make_unique<Shader>("res/shaders/toy.shader");

	quad = std::make_unique<FullscreenQuad>();

	m_Texture = std::make_unique<Texture>(R"(res/Textures/1.png)");
	m_Shader->Bind();
	m_Shader->setUniform1i("u_Texture", 0); // Link shader uniform to texture unit 0
	m_Shader->setUniform2f("u_Texel", m_Texture->getTexelSize().x, m_Texture->getTexelSize().y);
	m_Shader->setUniform1i("u_Effect", m_Effect); // Initial effect (None)
	m_Shader->setUniform1f("u_Opacity", m_Opacity);
	m_Shader->Unbind();
	m_Texture->Unbind();
}

void test::testShaderToy::Update(float deltaTime)
{
	

}

void test::testShaderToy::Render()
{
	Renderer renderer;
	renderer.Clear();
	m_Shader->Bind();
	m_Texture->Bind();
	m_Shader->setUniform1i("u_Effect", m_Effect); // Pass selected effect to shader
	m_Shader->setUniform1f("u_Opacity", m_Opacity);
	quad->Draw();
	m_Shader->Unbind();
	m_Texture->Unbind();
}

void test::testShaderToy::RenderGUI()
{

	ImGui::Text("Select Post-Processing Effect:");

	// Updated effect names list
	const char* effects[] = {
		"None",
		"Edge Detection",
		"Color Inversion",
		"Blur",
		"Sharpen",
		"Bloom",
		"Grayscale",
		"Left Blue / Right Yellow",
		"Top Green / Bottom Blue",
		"Center Red / Outer Orange"
	};
	ImGui::Combo("Effect", &m_Effect, effects, IM_ARRAYSIZE(effects));

	// Opacity slider (0.0 = no effect, 1.0 = full effect)
	ImGui::SliderFloat("Opacity", &m_Opacity, 0.0f, 1.0f);

}
