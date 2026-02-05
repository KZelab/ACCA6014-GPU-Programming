#pragma once
#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include "../utils/Camera.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

namespace test {
    class TestCamera : public Tests {
    public:
        TestCamera(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        std::unique_ptr<Camera> m_Camera;
        std::unique_ptr<Mesh> m_Cube;
        std::unique_ptr<Shader> m_Shader;
    };
}
