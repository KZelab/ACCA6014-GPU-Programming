#include "TestShadowMapping.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

test::TestShadowMapping::TestShadowMapping(GLFWwindow* window)
	: m_Window(window),
	  m_LightDirection(glm::normalize(glm::vec3(0.5f, -1.0f, 0.3f))),
	  m_LightColour(1.0f, 1.0f, 1.0f),
	  m_ShadowBias(0.005f),
	  m_ShadowResolution(2048),
	  m_EnablePCF(true),
	  m_PCFKernelSize(3),
	  m_ShowDebugShadowMap(false),
	  m_AmbientIntensity(0.15f),
	  m_DiffuseIntensity(0.8f),
	  m_SpecularIntensity(0.3f),
	  m_Shininess(32.0f),
	  m_ObjectColor(0.7f, 0.7f, 0.7f),
	  m_OrthoSize(20.0f),
	  m_NearPlane(0.1f),
	  m_FarPlane(50.0f)
{
	m_Camera = std::make_unique<Camera>(
		window,
		glm::vec3(0.0f, 8.0f, 20.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-90.0f,
		-15.0f,
		45.0f
	);

	m_DepthShader = std::make_unique<Shader>("res/Shaders/Shadows/ShadowDepth.shader");
	m_PhongShader = std::make_unique<Shader>("res/Shaders/Shadows/ShadowPhong.shader");

	// Shadow framebuffer
	m_ShadowFBO = std::make_unique<Framebuffer>(m_ShadowResolution, m_ShadowResolution, true);

	// Ground plane: flat slab
	m_GroundPlane = GeometryFactory::CreateCube();
	m_GroundPlane->setPosition(glm::vec3(0.0f, -0.05f, 0.0f));
	m_GroundPlane->setScale(glm::vec3(200.0f, 0.1f, 200.0f));

	// Cubes at varying positions
	m_Cubes[0] = GeometryFactory::CreateCube();
	m_Cubes[0]->setPosition(glm::vec3(-3.0f, 1.0f, 0.0f));
	m_Cubes[0]->setScale(glm::vec3(1.5f, 2.0f, 1.5f));

	m_Cubes[1] = GeometryFactory::CreateCube();
	m_Cubes[1]->setPosition(glm::vec3(2.0f, 0.75f, -2.0f));
	m_Cubes[1]->setScale(glm::vec3(1.0f, 1.5f, 1.0f));

	m_Cubes[2] = GeometryFactory::CreateCube();
	m_Cubes[2]->setPosition(glm::vec3(0.0f, 2.5f, 3.0f));
	m_Cubes[2]->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	m_Cubes[2]->setRotation(glm::vec3(0.0f, 45.0f, 0.0f));

	// Spheres
	m_Spheres[0] = GeometryFactory::CreateSphere(20, 20);
	m_Spheres[0]->setPosition(glm::vec3(4.0f, 1.0f, 2.0f));
	m_Spheres[0]->setScale(glm::vec3(1.0f, 1.0f, 1.0f));

	m_Spheres[1] = GeometryFactory::CreateSphere(20, 20);
	m_Spheres[1]->setPosition(glm::vec3(-1.5f, 0.75f, -4.0f));
	m_Spheres[1]->setScale(glm::vec3(0.75f, 0.75f, 0.75f));


	glEnable(GL_DEPTH_TEST);
}

test::TestShadowMapping::~TestShadowMapping()
{
	glDisable(GL_CULL_FACE);
}

void test::TestShadowMapping::Update(float deltaTime)
{
	m_Camera->processInput(deltaTime);
	m_Camera->Update(deltaTime);

	m_View = m_Camera->getViewMatrix();
	m_Projection = glm::perspective(glm::radians(m_Camera->getFOV()), 1920.0f / 1080.0f, 0.1f, 1000.0f);

	// Compute light space matrix (directional light -> orthographic)
	glm::vec3 lightPos = -m_LightDirection * 20.0f;
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProj = glm::ortho(-m_OrthoSize, m_OrthoSize, -m_OrthoSize, m_OrthoSize, m_NearPlane, m_FarPlane);
	m_LightSpaceMatrix = lightProj * lightView;
}

void test::TestShadowMapping::RenderScene(Shader& shader)
{
	// Ground
	shader.setUniformMat4f("u_Model", m_GroundPlane->getTransformMatrix());
	m_GroundPlane->Draw();

	// Cubes
	for (int i = 0; i < 3; i++)
	{
		shader.setUniformMat4f("u_Model", m_Cubes[i]->getTransformMatrix());
		m_Cubes[i]->Draw();
	}

	// Spheres
	for (int i = 0; i < 2; i++)
	{
		shader.setUniformMat4f("u_Model", m_Spheres[i]->getTransformMatrix());
		m_Spheres[i]->Draw();
	}
}

void test::TestShadowMapping::Render()
{
	// ========================================
	// Pass 1: Shadow depth pass
	// ========================================
	m_ShadowFBO->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	// Cull front faces during shadow pass to reduce shadow acne
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	m_DepthShader->Bind();
	m_DepthShader->setUniformMat4f("u_LightSpaceMatrix", m_LightSpaceMatrix);

	RenderScene(*m_DepthShader);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);

	m_ShadowFBO->Unbind();

	// Restore viewport to window size
	glViewport(0, 0, 1920, 1080);

	// ========================================
	// Pass 2: Scene rendering with shadows
	// ========================================
	Renderer renderer;
	renderer.Clear();

	m_PhongShader->Bind();

	// Bind shadow map to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ShadowFBO->GetDepthTexture());
	m_PhongShader->setUniform1i("u_ShadowMap", 0);

	// Camera/transform uniforms
	m_PhongShader->setUniformMat4f("u_View", m_View);
	m_PhongShader->setUniformMat4f("u_Projection", m_Projection);
	m_PhongShader->setUniformMat4f("u_LightSpaceMatrix", m_LightSpaceMatrix);

	// Light uniforms
	m_PhongShader->setUniform3f("u_Light.Direction", m_LightDirection.x, m_LightDirection.y, m_LightDirection.z);
	m_PhongShader->setUniform3f("u_Light.Colour", m_LightColour.r, m_LightColour.g, m_LightColour.b);

	glm::vec3 camPos = m_Camera->getPosition();
	m_PhongShader->setUniform3f("u_CameraPosition", camPos.x, camPos.y, camPos.z);

	// Phong params
	m_PhongShader->setUniform1f("u_AmbientIntensity", m_AmbientIntensity);
	m_PhongShader->setUniform1f("u_DiffuseIntensity", m_DiffuseIntensity);
	m_PhongShader->setUniform1f("u_SpecularIntensity", m_SpecularIntensity);
	m_PhongShader->setUniform1f("u_Shininess", m_Shininess);

	// Object color
	m_PhongShader->setUniform3f("u_ObjectColor", m_ObjectColor.r, m_ObjectColor.g, m_ObjectColor.b);

	// Shadow params
	m_PhongShader->setUniform1f("u_ShadowBias", m_ShadowBias);
	m_PhongShader->setUniform1i("u_EnablePCF", m_EnablePCF ? 1 : 0);
	m_PhongShader->setUniform1i("u_PCFKernelSize", m_PCFKernelSize);

	RenderScene(*m_PhongShader);

}

void test::TestShadowMapping::RenderGUI()
{
	ImGui::Text("Shadow Mapping Demo");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Separator();

	// Light direction
	ImGui::Text("Light Direction");
	ImGui::SliderFloat("Dir X", &m_LightDirection.x, -1.0f, 1.0f);
	ImGui::SliderFloat("Dir Y", &m_LightDirection.y, -1.0f, -0.1f);
	ImGui::SliderFloat("Dir Z", &m_LightDirection.z, -1.0f, 1.0f);
	m_LightDirection = glm::normalize(m_LightDirection);

	ImGui::Separator();
	ImGui::Text("Shadow Settings");

	// Shadow bias
	ImGui::SliderFloat("Shadow Bias", &m_ShadowBias, 0.0f, 0.05f, "%.4f");

	// Shadow resolution
	ImGui::Text("Shadow Map Resolution:");
	int prevRes = m_ShadowResolution;
	ImGui::RadioButton("1024", &m_ShadowResolution, 1024); ImGui::SameLine();
	ImGui::RadioButton("2048", &m_ShadowResolution, 2048); ImGui::SameLine();
	ImGui::RadioButton("4096", &m_ShadowResolution, 4096);
	if (m_ShadowResolution != prevRes)
	{
		RecreateShadowMap();
	}

	// PCF
	ImGui::Checkbox("Enable PCF", &m_EnablePCF);
	if (m_EnablePCF)
	{
		ImGui::Text("PCF Kernel Size:");
		ImGui::RadioButton("3x3", &m_PCFKernelSize, 3); ImGui::SameLine();
		ImGui::RadioButton("5x5", &m_PCFKernelSize, 5);
	}



	ImGui::Separator();
	ImGui::Text("Light Ortho Settings");
	ImGui::SliderFloat("Ortho Size", &m_OrthoSize, 5.0f, 50.0f);
	ImGui::SliderFloat("Near Plane", &m_NearPlane, 0.1f, 10.0f);
	ImGui::SliderFloat("Far Plane", &m_FarPlane, 10.0f, 100.0f);

	ImGui::Separator();
	ImGui::Text("Phong Lighting");
	ImGui::SliderFloat("Ambient", &m_AmbientIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse", &m_DiffuseIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &m_SpecularIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Shininess", &m_Shininess, 1.0f, 128.0f);

	ImGui::Separator();
	ImGui::ColorEdit3("Object Color", glm::value_ptr(m_ObjectColor));
	ImGui::ColorEdit3("Light Colour", glm::value_ptr(m_LightColour));
}

void test::TestShadowMapping::RecreateShadowMap()
{
	m_ShadowFBO = std::make_unique<Framebuffer>(m_ShadowResolution, m_ShadowResolution, true);
}
