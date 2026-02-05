#include "testEffects.h"
#include "imgui.h"

test::testEffects::testEffects(GLFWwindow* window)
{
	m_Shader = std::make_unique<Shader>("res/shaders/effect.shader");

	m_Quad = GeometryFactory::CreateFullscreenQuad();

	m_Texture = std::make_unique<Texture>("res/Textures/1.png");

	m_Shader->Bind();
	m_Shader->setUniform1i("u_Texture", 0); //texture unit 0
	m_Shader->setUniform2f("u_Texel", m_Texture->getTexelSize().x, m_Texture->getTexelSize().y);
	m_Shader->setUniform1i("u_Effect", m_Effect);
	m_Shader->setUniform1f("u_Opacity", m_Opacity);

	m_Shader->Unbind();
	m_Texture->Unbind();

}

void test::testEffects::Update(float deltaTime)
{
}

void test::testEffects::Render()
{
	
	Renderer renderer;
	renderer.Clear();

	m_Shader->Bind();
	m_Texture->Bind();
	m_Shader->setUniform1i("u_Effect", m_Effect);
	m_Shader->setUniform1f("u_Opacity", m_Opacity);
	m_Quad->Draw();
	m_Shader->Unbind();
	m_Texture->Unbind();
}

void test::testEffects::RenderGUI()
{
	ImGui::Text("Select post processing effect");

	const char* effects[]
		=
	{
		"None",
		"Edge detection",
		"Colour inversion",
		"Blur",
		"Sharpen",
		"Bloom",
		"Greyscale",
		"Venuezela",
		"Romania",
		"middle box"
	};

	ImGui::Combo("Effects", &m_Effect, effects, IM_ARRAYSIZE(effects));

	//opacity slider (0 will be no effect applied and 1 will be full effect)
	ImGui::SliderFloat("Opacity", &m_Opacity, 0.0, 1.0);


}
