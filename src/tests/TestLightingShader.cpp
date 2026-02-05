#include "TestLightingShader.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

test::TestLightingShader::TestLightingShader(GLFWwindow* window)
	: m_Window(window),
	  m_AmbientIntensity(0.2f),
	  m_DiffuseIntensity(0.7f),
	  m_SpecularIntensity(0.8f),
	  m_Shininess(32.0f),
	  m_LightPosition(10.0f, 15.0f, 25.0f),
	  m_LightColour(1.0f, 1.0f, 1.0f)
{
	m_Camera = std::make_unique<Camera>(
		window,
		glm::vec3(0.0f, 0.0f, 5.0f),  // position
		glm::vec3(0.0f, 1.0f, 0.0f),  // up vector
		-90.0f,                        // yaw
		0.0f,                          // pitch
		45.0f                          // FOV
	);

	m_PhongShader = std::make_unique<Shader>("res/shaders/Phong.shader");
	m_FlatShader = std::make_unique<Shader>("res/shaders/Flat.shader");
	m_GouraudShader = std::make_unique<Shader>("res/shaders/Gouraud.shader");
	m_BlinnPhongShader = std::make_unique<Shader>("res/shaders/Blinn-Phong.shader");

	m_Sphere = GeometryFactory::CreateSphere(20, 20);

	glEnable(GL_DEPTH_TEST);
}

void test::TestLightingShader::Update(float deltaTime)
{
	m_Camera->processInput(deltaTime);
	m_Camera->Update(deltaTime);

	m_Model = glm::mat4(1.0f);
	float angle = static_cast<float>(glfwGetTime()) * 20.0f;
	m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));

	m_View = m_Camera->getViewMatrix();
	m_Projection = glm::perspective(glm::radians(m_Camera->getFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
}

void test::TestLightingShader::Render()
{
	Renderer renderer;
	renderer.Clear();

	Shader* shader = nullptr;
	switch (m_CurrentShader)
	{
	case 0: shader = m_PhongShader.get(); break;
	case 1: shader = m_FlatShader.get(); break;
	case 2: shader = m_GouraudShader.get(); break;
	case 3: shader = m_BlinnPhongShader.get(); break;
	default: shader = m_PhongShader.get();
	}

	shader->Bind();
	shader->setUniformMat4f("u_Model", m_Model);
	shader->setUniformMat4f("u_View", m_View);
	shader->setUniformMat4f("u_Projection", m_Projection);

	shader->setUniform3f("u_Light.Position", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
	shader->setUniform3f("u_Light.Colour", m_LightColour.r, m_LightColour.g, m_LightColour.b);

	glm::vec3 camPos = m_Camera->getPosition();
	shader->setUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

	shader->setUniform1f("u_AmbientIntensity", m_AmbientIntensity);
	shader->setUniform1f("u_DiffuseIntensity", m_DiffuseIntensity);
	shader->setUniform1f("u_SpecularIntensity", m_SpecularIntensity);
	shader->setUniform1f("u_Shininess", m_Shininess);

	m_Sphere->setPosition(glm::vec3(0, 0, 0));
	m_Sphere->Draw();
}

void test::TestLightingShader::RenderGUI()
{
	ImGui::Text("Lighting Parameters");
	ImGui::SliderFloat("Ambient Intensity", &m_AmbientIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Intensity", &m_DiffuseIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Intensity", &m_SpecularIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Shininess", &m_Shininess, 1.0f, 128.0f);

	ImGui::Separator();
	ImGui::Text("Light Properties");
	ImGui::SliderFloat3("Light Position", glm::value_ptr(m_LightPosition), -20.0f, 20.0f);
	ImGui::ColorEdit3("Light Colour", glm::value_ptr(m_LightColour));

	ImGui::Text("Light Presets:");
	if (ImGui::Button("Neutral White")) {
		m_LightColour = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Cool Blue")) {
		m_LightColour = glm::vec3(0.8f, 0.9f, 1.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Warm Yellow")) {
		m_LightColour = glm::vec3(1.0f, 0.9f, 0.8f);
	}

	ImGui::Separator();
	ImGui::Text("Shader Selection");
	const char* shaders[] = { "Phong", "Flat", "Gouraud", "Blinn-Phong" };
	ImGui::Combo("Active Shader", &m_CurrentShader, shaders, IM_ARRAYSIZE(shaders));

	if (ImGui::Checkbox("Wireframe Mode", &m_Wireframe)) {
		glPolygonMode(GL_FRONT_AND_BACK, m_Wireframe ? GL_LINE : GL_FILL);
	}
}
