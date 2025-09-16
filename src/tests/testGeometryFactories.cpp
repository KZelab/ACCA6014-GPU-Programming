#include "testGeometryFactories.h"
#include <GL/glew.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace test
{
    testGeometryFactories::testGeometryFactories()
        : m_CurrentMesh(nullptr), m_Shader(nullptr), m_Renderer(nullptr),
          m_CurrentGeometry(TRIANGLE), m_ShowWireframe(false),
          m_RotationSpeed(50.0f), m_CurrentRotation(0.0f), m_Scale(1.0f)
    {
        m_Renderer = new Renderer();

        // Create shader for rendering geometry
        std::string vertexShader = R"(
            #version 330 core
            layout(location = 0) in vec3 aPos;
            layout(location = 1) in vec3 aNormal;
            layout(location = 2) in vec3 aColor;
            layout(location = 3) in vec2 aTexCoord;

            uniform mat4 u_MVP;

            out vec3 FragColor;
            out vec3 FragNormal;
            out vec2 TexCoord;

            void main()
            {
                gl_Position = u_MVP * vec4(aPos, 1.0);
                FragColor = aColor;
                FragNormal = aNormal;
                TexCoord = aTexCoord;
            }
        )";

        std::string fragmentShader = R"(
            #version 330 core
            out vec4 color;

            in vec3 FragColor;
            in vec3 FragNormal;
            in vec2 TexCoord;

            void main()
            {
                // Simple lighting using normals for visual interest
                vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
                float lighting = max(dot(normalize(FragNormal), lightDir), 0.3);
                color = vec4(FragColor * lighting, 1.0);
            }
        )";

        m_Shader = new Shader(vertexShader, fragmentShader);

        // Initialize with triangle
        UpdateMesh();
    }

    testGeometryFactories::~testGeometryFactories()
    {
        delete m_CurrentMesh;
        delete m_Shader;
        delete m_Renderer;
    }

    void testGeometryFactories::Update(float deltaTime)
    {
        m_CurrentRotation += m_RotationSpeed * deltaTime;
        if (m_CurrentRotation > 360.0f) {
            m_CurrentRotation -= 360.0f;
        }
    }

    void testGeometryFactories::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        if (m_ShowWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (m_CurrentMesh && m_Shader) {
            m_Shader->Bind();

            // Create model matrix with rotation and scaling
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(m_Scale));

            if (m_CurrentGeometry == CUBE) {
                // Rotate cube around both X and Y axes for better visibility
                model = glm::rotate(model, glm::radians(m_CurrentRotation), glm::vec3(1.0f, 1.0f, 0.0f));
            } else {
                // Rotate 2D shapes around Z axis
                model = glm::rotate(model, glm::radians(m_CurrentRotation), glm::vec3(0.0f, 0.0f, 1.0f));
            }

            // Create view and projection matrices
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f);

            glm::mat4 mvp = projection * view * model;
            m_Shader->SetUniformMat4f("u_MVP", mvp);

            m_Renderer->Draw(*m_CurrentMesh, *m_Shader);
            m_Shader->Unbind();
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);
    }

    void testGeometryFactories::RenderGUI()
    {
        ImGui::Text("Geometry Factory Test");
        ImGui::Separator();

        // Geometry selection
        const char* geometryTypes[] = { "Triangle", "Quad", "Cube", "Sphere", "Fullscreen Quad", "Cylinder", "Plane" };
        int currentItem = static_cast<int>(m_CurrentGeometry);
        if (ImGui::Combo("Geometry Type", &currentItem, geometryTypes, 7)) {
            SwitchGeometry(static_cast<GeometryType>(currentItem));
        }

        ImGui::Separator();

        // Rendering options
        if (ImGui::Checkbox("Wireframe", &m_ShowWireframe)) {
            // Checkbox state updated automatically
        }

        if (ImGui::SliderFloat("Rotation Speed", &m_RotationSpeed, 0.0f, 200.0f)) {
            // Slider value updated automatically
        }

        if (ImGui::SliderFloat("Scale", &m_Scale, 0.1f, 3.0f)) {
            // Slider value updated automatically
        }

        ImGui::Separator();

        // Information display
        if (m_CurrentMesh) {
            ImGui::Text("Index Count: %u", m_CurrentMesh->GetIndexCount());
        }

        ImGui::Text("Current Rotation: %.1f degrees", m_CurrentRotation);

        // Educational information
        ImGui::Separator();
        ImGui::Text("Educational Notes:");
        switch (m_CurrentGeometry) {
            case TRIANGLE:
                ImGui::BulletText("3 vertices, 1 triangle (3 indices)");
                ImGui::BulletText("Fundamental primitive in 3D graphics");
                ImGui::BulletText("All complex geometry built from triangles");
                break;
            case QUAD:
                ImGui::BulletText("4 vertices, 2 triangles (6 indices)");
                ImGui::BulletText("Demonstrates index buffer optimization");
                ImGui::BulletText("Reuses vertices between triangles");
                break;
            case CUBE:
                ImGui::BulletText("8 vertices, 12 triangles (36 indices)");
                ImGui::BulletText("Shows 3D geometry construction");
                ImGui::BulletText("Each face uses different vertex colors");
                break;
            case SPHERE:
                ImGui::BulletText("Parametric surface generation");
                ImGui::BulletText("Demonstrates advanced geometry algorithms");
                ImGui::BulletText("Perfect normals for lighting calculations");
                break;
            case FULLSCREEN_QUAD:
                ImGui::BulletText("NDC coordinates (-1 to 1)");
                ImGui::BulletText("Used for post-processing effects");
                ImGui::BulletText("No transformation needed");
                break;
            case CYLINDER:
                ImGui::BulletText("Parametric cylinder generation");
                ImGui::BulletText("Demonstrates advanced UV mapping");
                ImGui::BulletText("Radial normal calculations");
                break;
            case PLANE:
                ImGui::BulletText("Tessellated plane with subdivisions");
                ImGui::BulletText("Shows tessellation concepts");
                ImGui::BulletText("Demonstrates mesh density control");
                break;
        }
    }

    void testGeometryFactories::SwitchGeometry(GeometryType type)
    {
        if (type != m_CurrentGeometry) {
            m_CurrentGeometry = type;
            UpdateMesh();
        }
    }

    void testGeometryFactories::UpdateMesh()
    {
        // Clean up old mesh
        if (m_CurrentMesh) {
            delete m_CurrentMesh;
            m_CurrentMesh = nullptr;
        }

        // Create new mesh based on current geometry type
        switch (m_CurrentGeometry) {
            case TRIANGLE:
                m_CurrentMesh = GeometryFactory::CreateTriangle();
                break;
            case QUAD:
                m_CurrentMesh = GeometryFactory::CreateQuad();
                break;
            case CUBE:
                m_CurrentMesh = GeometryFactory::CreateCube();
                break;
            case SPHERE:
                m_CurrentMesh = GeometryFactory::CreateSphere(20, 20);
                break;
            case FULLSCREEN_QUAD:
                m_CurrentMesh = GeometryFactory::CreateFullscreenQuad();
                break;
            case CYLINDER:
                m_CurrentMesh = GeometryFactory::CreateCylinder(20, 1, 1.0f, 0.5f);
                break;
            case PLANE:
                m_CurrentMesh = GeometryFactory::CreatePlane(10, 10, 2.0f, 2.0f);
                break;
        }
    }
}