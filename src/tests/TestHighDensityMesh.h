#pragma once
#include "Tests.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"


#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include "glm/glm.hpp"

namespace test {
    class TestHighDensityMesh : public Tests {
    public:
        TestHighDensityMesh(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        // Helper function to load a model
        void LoadModel(const std::string& filepath);

        GLFWwindow* m_window;

        // Camera parameters (you can copy camera control logic from TestCamera)
        glm::vec3 m_CameraPos;
        glm::vec3 m_CameraFront;
        glm::vec3 m_CameraUp;
        float m_CameraSpeed;

        // Transformations
        glm::mat4 m_Model;
        glm::mat4 m_View;
        glm::mat4 m_Projection;

        // GPU Buffers
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Shader> m_Shader;

        // Mesh data
        std::vector<float> m_Vertices;   // Interleaved: positions, normals, texCoords etc.
        std::vector<unsigned int> m_Indices;

        // Rendering parameters
        float m_FOV;
        float m_ModelRotationSpeed;

        void ProcessInput();
    };
}
