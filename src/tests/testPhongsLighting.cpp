#include "testPhongsLighting.h"

#include <glm/gtc/type_ptr.inl>

#include "imgui/imgui.h"
#include "MeshGenerator/Sphere.h"


test::testPhongsLighting::testPhongsLighting(GLFWwindow* window) :
    camera(window, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 45.0f), // Adjusted camera position
    m_AmbientIntensity(0.2f),  // Balanced ambient light
    m_DiffuseIntensity(0.7f),  // Stronger diffuse light for depth
    m_SpecularIntensity(0.8f), // Controlled specular reflections
    m_Shininess(32.0f),        // Default shininess for plastic-like material
    m_LightPosition(glm::vec3(10.0f, 15.0f, 25.0f)), // More natural light position
    m_LightColour(glm::vec3(1.0f, 1.0f, 1.0f)), // Neutral white light
    m_window(window),
    m_CurrentShader(0) // Default to Phong shade

{
	// Load Phongs lighting shaders and shading
	m_Shader = std::make_unique<Shader>("res/shaders/Phong.shader");
    // Load Phongs lighting with flat shading 
    m_Shader2 = std::make_unique<Shader>("res/shaders/Flat.shader");
    // Load Phongs lighting with Gouraund shading 
    m_Shader3 = std::make_unique<Shader>("res/shaders/Gouraud.shader");
    //load Phongs Lighting with blinn-phong shading
    m_Shader4 = std::make_unique<Shader>("res/shaders/Blinn-Phong.shader");
    sphere = std::make_unique<Sphere>(10, 10);


    m_Shader->Unbind();
    m_Shader2->Unbind();
    m_Shader3->Unbind();

    glEnable(GL_DEPTH_TEST);


}

void test::testPhongsLighting::Update(float deltaTime)
{
    camera.processInput(deltaTime);
    camera.Update(deltaTime);

    m_Model = glm::mat4(1.0f); 
    float angle = glfwGetTime() * 20.0f; // Rotate at 20 degrees per second
    m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));
    m_View = camera.GetViewMatrix();
    m_Projection = glm::perspective(glm::radians(camera.GetFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
}


void test::testPhongsLighting::Render()
{
    Renderer renderer;
    renderer.Clear();


    Shader* activeShader = nullptr;

    // Select the active shader based on m_CurrentShader
    switch (m_CurrentShader) {
    case 0:
        activeShader = m_Shader.get(); // Phong Shader
        break;
    case 1:
        activeShader = m_Shader2.get(); // Flat Shader
        break;
    case 2:
        activeShader = m_Shader3.get(); // Gouraud Shader
        break;
    case 3:
        activeShader = m_Shader4.get(); // Blinn Phong Shader
        break;
    default:
        activeShader = m_Shader.get();
    }

    activeShader->Bind();
    activeShader->setUniformMat4f("u_Model", m_Model);
    activeShader->setUniformMat4f("u_View", m_View);
    activeShader->setUniformMat4f("u_Projection", m_Projection);

    activeShader->setUniform3f("uLight.Position", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);
    activeShader->setUniform3f("uLight.Colour", m_LightColour.r, m_LightColour.g, m_LightColour.b);
    activeShader->setUniform3f("uCameraPosition", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

    activeShader->setUniform1f("uAmbientIntensity", m_AmbientIntensity);
    activeShader->setUniform1f("uDiffuseIntensity", m_DiffuseIntensity);
    activeShader->setUniform1f("uSpecularIntensity", m_SpecularIntensity);
    activeShader->setUniform1f("uShininess", m_Shininess);
    sphere->setPosition(glm::vec3(0, 0, 0));
    sphere->Draw();
}

void test::testPhongsLighting::RenderGUI()
{
    ImGui::Text("Phong Lighting Parameters");

    ImGui::SliderFloat("Ambient Intensity", &m_AmbientIntensity, 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Intensity", &m_DiffuseIntensity, 0.0f, 1.0f);
    ImGui::SliderFloat("Specular Intensity", &m_SpecularIntensity, 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &m_Shininess, 1.0f, 128.0f);

    ImGui::Text("Light Properties");

    // Light Position Control
    ImGui::SliderFloat3("Light Position", glm::value_ptr(m_LightPosition), -10.0f, 10.0f);

    // Light Colour Picker
    ImGui::ColorEdit3("Light Colour", glm::value_ptr(m_LightColour));

    // Preset Buttons for Light Colour
    ImGui::Text("Quick Light Presets:");

    if (ImGui::Button("Neutral White")) {
        m_LightColour = glm::vec3(1.0f, 1.0f, 1.0f); // White light
    }
    ImGui::SameLine(); // Places buttons in the same row

    if (ImGui::Button("Cool Blue")) {
        m_LightColour = glm::vec3(0.8f, 0.9f, 1.0f); // Slightly bluish
    }
    ImGui::SameLine();

    if (ImGui::Button("Warm Yellow")) {
        m_LightColour = glm::vec3(1.0f, 0.9f, 0.8f); // Slightly yellowish
    }

    // Add Shader Selector Combo Box
    ImGui::Text("Shader Selection:");
    const char* shaders[] = { "Phong Shading", "Flat Shading", "Gouraud Shading", "Blinn Phong" };
    ImGui::Combo("Active Shader", &m_CurrentShader, shaders, IM_ARRAYSIZE(shaders));

    static bool wireframe = false;
    if (ImGui::Checkbox("Wireframe Mode", &wireframe)) {
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }
}




