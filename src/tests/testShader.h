#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <string>
#include "Tests.h"
#include "../buffers/VertexBuffer.h"
#include "../buffers/VertexBufferLayout.h"
#include "../buffers/IndexBuffer.h"
#include "../buffers/VertexArray.h"
#include "../buffers/Shader.h"
#include "buffers/Texture.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"


namespace test {

    class testShader : public Tests{
    public:
        testShader(GLFWwindow* window);

        void Update(float deltaTime);
        void Render();
        void RenderGUI();

    private:
        void ProcessInput(float deltaTime);

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Texture> m_Texture;


        // Variables for uniforms
        glm::vec2 m_Resolution;
        glm::vec3 m_CameraPos;

        // Camera and input
        GLFWwindow* m_Window;


        // Vertex and index data for a full-screen quad
        std::vector<float> m_Vertices;
        std::vector<unsigned int> m_Indices;
    };

} // namespace test
