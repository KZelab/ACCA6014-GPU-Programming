#include "TestHighDensityMesh.h"
#include "../Renderer/Renderer.h"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
// Include your model loading library headers here, for example Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <imgui.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "../buffers/VertexBufferLayout.h"
namespace test {

    TestHighDensityMesh::TestHighDensityMesh(GLFWwindow* window)
        : m_window(window),
        m_CameraPos(0.0f, 0.0f, 3.0f),
        m_CameraFront(0.0f, 0.0f, -1.0f),
        m_CameraUp(0.0f, 1.0f, 0.0f),
        m_CameraSpeed(2.5f),
        m_FOV(45.0f),
        m_ModelRotationSpeed(0.5f)
    {
        // Enable Depth Test
        glEnable(GL_DEPTH_TEST);

        // Load model data
        LoadModel("res/Models/poly.obj");

        // Create and bind buffers
        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float));
        m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), (unsigned int)m_Indices.size());

        // Set up layout according to how we populated m_Vertices
        // Suppose each vertex is laid out as: position (3 floats), normal (3 floats), texCoord (2 floats)
        VertexBufferLayout layout;
        layout.Push<float>(3); // position
        layout.Push<float>(3); // normal
        layout.Push<float>(2); // texCoords
        m_VAO->AddBuffer(*m_VBO, layout);

        m_Shader = std::make_unique<Shader>("res/Shaders/Mesh.shader");
        m_Shader->Bind();

        m_VAO->unBind();
        m_VBO->Unbind();
        m_IBO->Unbind();
        m_Shader->Unbind();
    }


    void TestHighDensityMesh::Update(float deltaTime) {
        ProcessInput();

        // Just rotate the model slowly around Y-axis for demonstration
        static float rotation = 0.0f;
        rotation += m_ModelRotationSpeed * deltaTime;
        m_Model = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 1, 0));

        m_View = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
        m_Projection = glm::perspective(glm::radians(m_FOV), 800.0f / 600.0f, 0.1f, 1000.0f);
    }

    void TestHighDensityMesh::Render() {
        Renderer renderer;
        renderer.Clear();

        m_Shader->Bind();
        m_Shader->setUniformMat4f("u_Model", m_Model);
        m_Shader->setUniformMat4f("u_View", m_View);
        m_Shader->setUniformMat4f("u_Projection", m_Projection);

        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
    }

    void TestHighDensityMesh::RenderGUI() {
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", m_CameraPos.x, m_CameraPos.y, m_CameraPos.z);
        ImGui::SliderFloat("Camera Speed", &m_CameraSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 90.0f);
        ImGui::SliderFloat("Model Rotation Speed", &m_ModelRotationSpeed, 0.0f, 2.0f);
    }

    void TestHighDensityMesh::ProcessInput() {
        GLFWwindow* window = glfwGetCurrentContext();
        float cameraSpeed = m_CameraSpeed * 0.1f; // scale based on delta if needed

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_CameraPos += cameraSpeed * m_CameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_CameraPos -= cameraSpeed * m_CameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * cameraSpeed;
    }

    void TestHighDensityMesh::LoadModel(const std::string& filepath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath,
            aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
            return;
        }

        // For simplicity, assume just one mesh
        aiMesh* mesh = scene->mMeshes[0];

        m_Vertices.reserve(mesh->mNumVertices * 8); // 3 pos + 3 norm + 2 tex
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            // Positions
            m_Vertices.push_back(mesh->mVertices[i].x);
            m_Vertices.push_back(mesh->mVertices[i].y);
            m_Vertices.push_back(mesh->mVertices[i].z);

            // Normals
            m_Vertices.push_back(mesh->mNormals[i].x);
            m_Vertices.push_back(mesh->mNormals[i].y);
            m_Vertices.push_back(mesh->mNormals[i].z);

            // TexCoords
            if (mesh->mTextureCoords[0]) {
                m_Vertices.push_back(mesh->mTextureCoords[0][i].x);
                m_Vertices.push_back(mesh->mTextureCoords[0][i].y);
            }
            else {
                m_Vertices.push_back(0.0f);
                m_Vertices.push_back(0.0f);
            }
        }

        // Indices
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                m_Indices.push_back(face.mIndices[j]);
            }
        }
    }

}
