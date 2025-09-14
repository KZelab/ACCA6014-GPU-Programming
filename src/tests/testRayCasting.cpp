#include "TestRayCasting.h"
#include "Renderer/Renderer.h"
#include "vendor/imgui/imgui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace test {
    TestRayCasting::TestRayCasting(GLFWwindow* window)
        : m_window(window), selectedObjectIndex(-1) {

        // Initialise camera
        cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraSpeed = 0.5f;

        // Example objects
        objects.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, "Obj 1" });
        objects.push_back({ glm::vec3(2.0f, 0.0f, -3.0f), 1.5f, "Obj 2" });
        objects.push_back({ glm::vec3(-2.0f, 1.0f, -2.0f), 1.0f, "Obj 3" });

        // Setup the VAO, VBO, and IBO for the cube/sphere
        SetupBuffers();

        // Load shader
        m_Shader = std::make_unique<Shader>("res/Shaders/ProjectionsShader2.shader");
    }

    void TestRayCasting::SetupBuffers() {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        GenerateSphereData(vertices, indices, 1.0f, 20, 20); // Radius = 1, 20 divisions each for longitude and latitude

        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(3); // Position (x, y, z)
        layout.Push<float>(3); // Normal (nx, ny, nz)
        m_VAO->AddBuffer(*m_VBO, layout);

        m_IBO = std::make_unique<IndexBuffer>(indices.data(), indices.size());

        // Enable depth testing for 3D object rendering
        glEnable(GL_DEPTH_TEST);
    }
    void TestRayCasting::GenerateSphereData(std::vector<float>& vertices, std::vector<unsigned int>& indices, float radius, unsigned int longitudeDiv, unsigned int latitudeDiv) {
        for (unsigned int lat = 0; lat <= latitudeDiv; ++lat) {
            float theta = lat * glm::pi<float>() / latitudeDiv;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            for (unsigned int lon = 0; lon <= longitudeDiv; ++lon) {
                float phi = lon * 2.0f * glm::pi<float>() / longitudeDiv;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);

                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;

                // Push vertex position
                vertices.push_back(radius * x);
                vertices.push_back(radius * y);
                vertices.push_back(radius * z);

                // Push vertex normal
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);
            }
        }

        // Create index data for the sphere using triangles
        for (unsigned int lat = 0; lat < latitudeDiv; ++lat) {
            for (unsigned int lon = 0; lon < longitudeDiv; ++lon) {
                unsigned int first = (lat * (longitudeDiv + 1)) + lon;
                unsigned int second = first + longitudeDiv + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }
    }

    void TestRayCasting::Update(float deltaTime) {
        ProcessInput();

        double mouseX, mouseY;
        glfwGetCursorPos(m_window, &mouseX, &mouseY);

        glm::vec3 rayDirection = CalculateRayDirection((float)mouseX, (float)mouseY);
        glm::vec3 rayOrigin = cameraPosition;

        // Find the first object the ray intersects
        selectedObjectIndex = -1;
        for (size_t i = 0; i < objects.size(); ++i) {
            if (RayIntersectsSphere(rayOrigin, rayDirection, objects[i].position, objects[i].radius)) {
                selectedObjectIndex = static_cast<int>(i);
                break;
            }
        }
    }

    void TestRayCasting::Render() {
        Renderer renderer;
        renderer.Clear();

        m_Shader->Bind();

        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        projectionMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        m_Shader->setUniformMat4f("view", viewMatrix);
        m_Shader->setUniformMat4f("projection", projectionMatrix);

        for (size_t i = 0; i < objects.size(); ++i) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), objects[i].position);
            m_Shader->setUniformMat4f("model", model);

            // Set the colour based on whether the object is selected
            glm::vec3 color = (selectedObjectIndex == static_cast<int>(i)) ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
            m_Shader->setUniform4f("u_Color", color.r, color.g, color.b, 1.0f);

            // Draw the object using the renderer
            renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
        }
    }

    void TestRayCasting::RenderGUI() {
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1f, 10.0f);

        if (selectedObjectIndex != -1) {
            const Object& selectedObject = objects[selectedObjectIndex];
            ImGui::Text("Selected Object: %s", selectedObject.name.c_str());
            ImGui::Text("Position: (%.1f, %.1f, %.1f)", selectedObject.position.x, selectedObject.position.y, selectedObject.position.z);
            ImGui::Text("Radius: %.1f", selectedObject.radius);
        }
        else {
            ImGui::Text("No object selected.");
        }
    }
    void TestRayCasting::ProcessInput() {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPosition += cameraSpeed * cameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPosition -= cameraSpeed * cameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    glm::vec3 TestRayCasting::CalculateRayDirection(float mouseX, float mouseY) {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        // Convert mouse position to NDC
        float x = (2.0f * mouseX) / width - 1.0f;
        float y = 1.0f - (2.0f * mouseY) / height; // Invert Y-axis
        float z = 1.0f;

        glm::vec4 rayClip(x, y, -1.0f, 1.0f); // In clip space
        glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
        rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f); // Convert to eye space
        glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
        return glm::normalize(rayWorld);
    }

    bool TestRayCasting::RayIntersectsSphere(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, const glm::vec3& sphereCenter, float sphereRadius) {
        glm::vec3 oc = rayOrigin - sphereCenter;
        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(oc, rayDirection);
        float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
        float discriminant = b * b - 4 * a * c;
        return discriminant > 0; // Intersection if discriminant > 0
    }
}
