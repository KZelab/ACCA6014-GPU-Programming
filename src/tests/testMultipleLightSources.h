#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include "../Renderer.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test {

    // Enum to represent different light types
    enum class LightType {
        Point,
        Directional,
        Spot
    };

    // Struct to store properties of each light
    struct Light {
        LightType type;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 colour;
        float intensity;
        float cutoff; // Used only for spotlights
    };

    class testMultipleLightSources : public Tests {
    public:
        testMultipleLightSources(GLFWwindow* window);
        ~testMultipleLightSources() override = default;

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_Window;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Mesh> m_Sphere;
        std::unique_ptr<Shader> m_Shader;

        // Material and scene lighting parameters
        float m_AmbientIntensity;
        float m_DiffuseIntensity;
        float m_SpecularIntensity;
        float m_Shininess;

        // Transformation matrices
        glm::mat4 m_Model;
        glm::mat4 m_View;
        glm::mat4 m_Projection;

        std::vector<Light> m_Lights; // All active lights
        int m_SelectedLightIndex = 0;  // Light being edited in ImGui
    };
}