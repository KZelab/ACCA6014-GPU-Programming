#pragma once
#include "Tests.h"
#include "../buffers/VertexBuffer.h"
#include "../buffers/VertexBufferLayout.h"
#include "../buffers/IndexBuffer.h"
#include "../buffers/VertexArray.h"
#include "../buffers/Shader.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <memory>

namespace test {
    class TestRayCasting : public Tests {
    public:
        TestRayCasting(GLFWwindow* window);
        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_window;

        glm::vec3 cameraPosition;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        float cameraSpeed;

        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        struct Object {
            glm::vec3 position;
            float radius; // For sphere objects
            std::string name; // Object name for GUI feedback
        };

        std::vector<Object> objects;
        int selectedObjectIndex; // Index of the currently selected object (-1 if none)

        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Shader> m_Shader;


        void GenerateSphereData(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int longitudeDiv, unsigned int latitudeDiv);


        void ProcessInput();
        void SetupBuffers();
        glm::vec3 CalculateRayDirection(float mouseX, float mouseY);
        bool RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, 
            const glm::vec3& sphereCenter, float sphereRadius);
    };
}
