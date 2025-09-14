#pragma once
#include "Tests.h"
#include "buffers/Shader.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "utils/Camera.h"
#include "glm/glm.hpp"
#include "MeshGenerator/Sphere.h"

namespace test {
    class testPhongsLighting : public Tests {
    public:
        testPhongsLighting(GLFWwindow* window);
        ~testPhongsLighting() override = default;

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        Camera camera;

        // Lighting parameters
        float m_AmbientIntensity;
        float m_DiffuseIntensity;
        float m_SpecularIntensity;
        float m_Shininess;

        // Light source properties
        glm::vec3 m_LightPosition;
        glm::vec3 m_LightColour;

        // Transformations
        glm::mat4 m_Model;
        glm::mat4 m_View;
        glm::mat4 m_Projection;



        std::unique_ptr<Sphere> sphere;



        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Shader> m_Shader2;
        std::unique_ptr<Shader> m_Shader3;
        std::unique_ptr<Shader> m_Shader4;
        int m_CurrentShader; // 0: Phong, 1: Flat, 2: Gouraud

    };
}
