#pragma once
#include "Tests.h"
#include "../buffers/VertexBuffer.h"
#include "../buffers/VertexBufferLayout.h"
#include "../buffers/IndexBuffer.h"
#include "../buffers/VertexArray.h"
#include "../buffers/Shader.h"
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

        glm::vec3 cameraPosition;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;

        float cameraSpeed;


        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<IndexBuffer> m_IndexBuffer;
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<VertexBuffer> m_VBO;

        void ProcessInput();
    };
}
