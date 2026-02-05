#include "testMultipleLightSources.h"

#include <glm/gtc/type_ptr.inl>

#include "../vendor/imgui/imgui.h"


test::testMultipleLightSources::testMultipleLightSources(GLFWwindow* window) :
    m_Window(window),
    m_AmbientIntensity(0.2f),
    m_DiffuseIntensity(0.7f),
    m_SpecularIntensity(0.8f),
    m_Shininess(32.0f)
{
    m_Camera = std::make_unique<Camera>(
        window,
        glm::vec3(0.0f, 0.0f, 5.0f),  // position
        glm::vec3(0.0f, 1.0f, 0.0f),  // up vector
        -90.0f,                        // yaw
        0.0f,                          // pitch
        45.0f                          // FOV
    );

    m_Shader = std::make_unique<Shader>("res/shaders/PhongMultiple.shader");
    m_Sphere = GeometryFactory::CreateSphere(20, 20);

    glEnable(GL_DEPTH_TEST);
}

void test::testMultipleLightSources::Update(float deltaTime)
{
    m_Camera->processInput(deltaTime);
    m_Camera->Update(deltaTime);

    m_Model = glm::mat4(1.0f);
    float angle = static_cast<float>(glfwGetTime()) * 20.0f;
    m_Model = glm::rotate(m_Model, glm::radians(angle), glm::vec3(0, 1, 0));
    m_View = m_Camera->getViewMatrix();
    m_Projection = glm::perspective(glm::radians(m_Camera->getFOV()), 800.0f / 600.0f, 0.1f, 1000.0f);
}


void test::testMultipleLightSources::Render()
{
    Renderer renderer;
    renderer.Clear();

    m_Shader->Bind();

    m_Shader->setUniformMat4f("u_Model", m_Model);
    m_Shader->setUniformMat4f("u_View", m_View);
    m_Shader->setUniformMat4f("u_Projection", m_Projection);

    glm::vec3 camPos = m_Camera->getPosition();
    m_Shader->setUniform3f("uCameraPosition", camPos.x, camPos.y, camPos.z);

    m_Shader->setUniform1f("uAmbientIntensity", m_AmbientIntensity);
    m_Shader->setUniform1f("uDiffuseIntensity", m_DiffuseIntensity);
    m_Shader->setUniform1f("uSpecularIntensity", m_SpecularIntensity);
    m_Shader->setUniform1f("uShininess", m_Shininess);

    m_Shader->setUniform1i("uLightCount", static_cast<int>(m_Lights.size()));
    for (size_t i = 0; i < m_Lights.size(); ++i) {
        const auto& light = m_Lights[i];
        std::string prefix = "uLights[" + std::to_string(i) + "]";
        m_Shader->setUniform3f(prefix + ".position", light.position.x, light.position.y, light.position.z);
        m_Shader->setUniform3f(prefix + ".direction", light.direction.x, light.direction.y, light.direction.z);
        m_Shader->setUniform3f(prefix + ".colour", light.colour.r, light.colour.g, light.colour.b);
        m_Shader->setUniform1f(prefix + ".intensity", light.intensity);
        m_Shader->setUniform1i(prefix + ".type", static_cast<int>(light.type));
        m_Shader->setUniform1f(prefix + ".cutoff", light.cutoff);
    }

    m_Sphere->setPosition(glm::vec3(0, 0, 0));
    m_Sphere->Draw();
}

void test::testMultipleLightSources::RenderGUI()
{
    ImGui::Text("Light Controls");
    if (ImGui::Button("Add Light")) {
        m_Lights.push_back({ LightType::Point, glm::vec3(10.0f, 15.0f, 25.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                             glm::vec3(1.0f), 1.0f, glm::cos(glm::radians(12.5f)) });
    }

    if (!m_Lights.empty()) {
        ImGui::SliderInt("Selected Light", &m_SelectedLightIndex, 0, static_cast<int>(m_Lights.size()) - 1);
        Light& light = m_Lights[m_SelectedLightIndex];

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
            m_Lights.erase(m_Lights.begin() + m_SelectedLightIndex);
            m_SelectedLightIndex = std::max(0, m_SelectedLightIndex - 1);
        }
    }
}




