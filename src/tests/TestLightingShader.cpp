#include "TestLightingShader.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
test::TestLightingShader::TestLightingShader(GLFWwindow* window):
	camera(window, 
		glm::vec3(0.0f, 0.0f, 5.0f), 
		glm::vec3(0.0f, 1.0f, 0.0f), 
		-90.0f, 
		0.0f, 
		45.0f),

	m_AmbientIntensity(0.2f),
	m_DiffuseIntensity(0.7f),
	m_SpecularIntensity(0.8f),
	m_Shininess(32.0f),

	m_LightPositon(glm::vec3(10.0f, 15.0f, 25.0f)),
	m_LightColour(glm::vec3(1.0f, 1.0f, 1.0f)),
	m_Window(window),
	m_CurrentShader(0)
{
	//load Phongs lighting shader with phong shading
	m_PhongsShader = std::make_unique<Shader>("res/shaders/Phong.shader");
	//load Phongs lighting shader with flat shading
	m_FlatShader = std::make_unique<Shader>("res/shaders/Flat.shader");
	//load Phongs lighting shader with gouraud shading
	m_Gouraud = std::make_unique<Shader>("res/shaders/Gouraud.shader");
	//load Phongs Lighting with blinn-phong shading
	m_BlinnPhong = std::make_unique<Shader>("res/shaders/Blinn-Phong.shader");

	sphere = std::make_unique<Sphere>(10, 10);//low poly sphere


	m_PhongsShader->Unbind();
	m_FlatShader->Unbind();
	m_Gouraud->Unbind();
	m_BlinnPhong->Unbind();

	glEnable(GL_DEPTH_TEST);
}

void test::TestLightingShader::Update(float deltaTime)
{
	camera.processInput(deltaTime);
	camera.Update(deltaTime);



	m_Model = glm::mat4(1.0f);
	float angle = glfwGetTime() * 20.0f;//rotate at 20 degrees per second
	m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));

	m_View = camera.getViewMatrix();

	m_Projection = glm::perspective(glm::radians(camera.getFOV()),
		800.0f / 600.0f, 0.1f, 1000.0f);

}

void test::TestLightingShader::Render()
{


	Shader* shader;
	switch (m_CurrentShader)
	{
	case 0:
		shader = m_PhongsShader.get();
		break;
	case 1:
		shader = m_FlatShader.get();
		break;
	case 2:
		shader = m_Gouraud.get();
		break;
	case 3:
		shader = m_BlinnPhong.get();
		break;
	default:
		shader = m_PhongsShader.get();
	}

	shader->Bind();
	shader->setUniformMat4f("u_Model", m_Model);
	shader->setUniformMat4f("u_View", m_View);
	shader->setUniformMat4f("u_Projection", m_Projection);


	shader->setUniform3f("u_Light.Position", m_LightPositon.x, m_LightPositon.y, m_LightPositon.z);
	shader->setUniform3f("u_Light.Colour", m_LightColour.r, m_LightColour.g, m_LightColour.b);
	shader->setUniform3f("u_CameraPosition", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

	shader->setUniform1f("u_AmbientIntensity",  m_AmbientIntensity);
	shader->setUniform1f("u_DiffuseIntensity",  m_DiffuseIntensity);
	shader->setUniform1f("u_SpecularIntensity", m_SpecularIntensity);
	shader->setUniform1f("u_Shininess",         m_Shininess);

	sphere->setPosition(glm::vec3(0, 0, 0));
	sphere->Draw();

}

void test::TestLightingShader::RenderGUI()
{
	ImGui::Text("Phong Lighting Paramaters");

	ImGui::SliderFloat("Ambient Intensity", &m_AmbientIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse Intensity", &m_DiffuseIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Intensity", &m_SpecularIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Shininess", &m_Shininess, 1.0f, 128.0f);


	ImGui::Text("Shader selector");
	const char* shaders[] = { "Phong", "Flat", "Gouraud", "Blinn-Phong" };

	ImGui::Combo("Active Shader", &m_CurrentShader, shaders, IM_ARRAYSIZE(shaders));

}
