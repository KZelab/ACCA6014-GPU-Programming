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
    class TestBatching : public Tests {
    public:
        TestBatching(GLFWwindow* window);
        ~TestBatching() override = default;

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        // Camera parameters (if you want to integrate with the camera you used above)
        glm::vec3 m_CameraPos;
        glm::vec3 m_CameraFront;
        glm::vec3 m_CameraUp;
        float m_CameraSpeed;

        // Batching resources
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Shader> m_Shader;

        // Configuration
        int m_GridSize;
        float m_Spacing;

        // Helper methods
        void GenerateBatchData(std::vector<float>& vertices, std::vector<unsigned int>& indices);
        void ProcessInput();
    };
}
