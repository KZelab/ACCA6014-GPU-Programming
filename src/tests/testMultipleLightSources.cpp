#include "testMultipleLightSources.h"

#include <glm/gtc/type_ptr.inl>

#include "imgui/imgui.h"
#include "MeshGenerator/Sphere.h"


test::testMultipleLightSources::testMultipleLightSources(GLFWwindow* window) :
    camera(window, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 45.0f), // Adjusted camera position
    m_AmbientIntensity(0.2f),  // Balanced ambient light
    m_DiffuseIntensity(0.7f),  // Stronger diffuse light for depth
    m_SpecularIntensity(0.8f), // Controlled specular reflections
    m_Shininess(32.0f),        // Default shininess for plastic-like material
    m_window(window)

{
    // Load Phongs lighting shaders and shading
    m_Shader = std::make_unique<Shader>("res/shaders/PhongMultiple.shader");

    sphere = std::make_unique<Sphere>(10, 10);


    m_Shader->Unbind();

    glEnable(GL_DEPTH_TEST);


}

void test::testMultipleLightSources::Update(float deltaTime)
{
    camera.processInput(deltaTime);
    camera.Update(deltaTime);

    m_Model = glm::mat4(1.0f);
    float angle = glfwGetTime() * 20.0f; // Rotate at 20 degrees per second
    m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));
    m_View = camera.GetViewMatrix();
    m_Projection = glm::perspective(glm::radians(camera.GetFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
}


void test::testMultipleLightSources::Render()
{
    Renderer renderer;
    renderer.Clear();

    m_Shader->Bind();

    m_Shader->setUniformMat4f("u_Model", m_Model);
    m_Shader->setUniformMat4f("u_View", m_View);
    m_Shader->setUniformMat4f("u_Projection", m_Projection);
    m_Shader->setUniform3f("uCameraPosition", camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

    m_Shader->setUniform1f("uAmbientIntensity", m_AmbientIntensity);
    m_Shader->setUniform1f("uDiffuseIntensity", m_DiffuseIntensity);
    m_Shader->setUniform1f("uSpecularIntensity", m_SpecularIntensity);
    m_Shader->setUniform1f("uShininess", m_Shininess);

    m_Shader->setUniform1i("uLightCount", static_cast<int>(m_Lights.size()));
    for (int i = 0; i < m_Lights.size(); ++i) {
        const auto& light = m_Lights[i];
        std::string prefix = "uLights[" + std::to_string(i) + "]";
        m_Shader->setUniform3f(prefix + ".position", light.position.x, light.position.y, light.position.z);
        m_Shader->setUniform3f(prefix + ".direction", light.direction.x, light.direction.y, light.direction.z);
        m_Shader->setUniform3f(prefix + ".colour", light.colour.r, light.colour.g, light.colour.b);
        m_Shader->setUniform1f(prefix + ".intensity", light.intensity);
        m_Shader->setUniform1i(prefix + ".type", static_cast<int>(light.type));
        m_Shader->setUniform1f(prefix + ".cutoff", light.cutoff);
    }

    sphere->setPosition(glm::vec3(0, 0, 0));
    sphere->Draw();
}

void test::testMultipleLightSources::RenderGUI()
{
    ImGui::Text("Light Controls");
    if (ImGui::Button("Add Light")) {
        m_Lights.push_back({ LightType::Point, glm::vec3(10.0f, 15.0f, 25.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                             glm::vec3(1.0f), 1.0f, glm::cos(glm::radians(12.5f)) });
    }

    if (!m_Lights.empty()) {
        ImGui::SliderInt("Selected Light", &selectedLightIndex, 0, static_cast<int>(m_Lights.size()) - 1);
        Light& light = m_Lights[selectedLightIndex];

        const char* types[] = { "Point", "Directional", "Spot" };
        int type = static_cast<int>(light.type);
        if (ImGui::Combo("Type", &type, types, IM_ARRAYSIZE(types))) {
            light.type = static_cast<LightType>(type);
        }

        ImGui::ColorEdit3("Colour", glm::value_ptr(light.colour));
        ImGui::SliderFloat3("Position", glm::value_ptr(light.position), -20.0f, 20.0f);
        if (light.type != LightType::Point) {
            ImGui::SliderFloat3("Direction", glm::value_ptr(light.direction), -1.0f, 1.0f);
        }
        ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 5.0f);
        if (light.type == LightType::Spot) {
            ImGui::SliderFloat("Cutoff Angle", &light.cutoff, 0.0f, 1.0f);
        }

        if (ImGui::Button("Remove Light")) {
            m_Lights.erase(m_Lights.begin() + selectedLightIndex);
            selectedLightIndex = std::max(0, selectedLightIndex - 1);
        }
    }
}




