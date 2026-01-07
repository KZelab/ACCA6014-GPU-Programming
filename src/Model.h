#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "glm/glm.hpp"

#include <vector>
#include <string>
#include <memory>

// Represents a 3D model loaded from file
class Model
{
public:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;

        Vertex() : position(0.0f), normal(0.0f), texCoord(0.0f) {}
        Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex)
            : position(pos), normal(norm), texCoord(tex) {}
    };

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    // OpenGL objects (created when mesh is uploaded to GPU)
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;

    // Texture (optional)
    std::unique_ptr<Texture> m_Texture;

    // Model properties
    glm::vec3 m_Position;
    glm::vec3 m_Rotation;  // Euler angles in degrees
    glm::vec3 m_Scale;

    // Bounding box (calculated during load)
    glm::vec3 m_BoundsMin;
    glm::vec3 m_BoundsMax;

public:
    Model();
    ~Model();

    // Load mesh data
    void SetMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Center the model at origin (horizontally) with feet at Y=0
    void CenterModelAtBase();

    // Upload mesh to GPU (call after SetMeshData)
    void UploadToGPU();

    // Load texture for model
    bool LoadTexture(const std::string& filepath);

    // Transform setters
    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }
    void SetScale(const glm::vec3& scale) { m_Scale = scale; }
    void SetScale(float uniformScale) { m_Scale = glm::vec3(uniformScale); }

    // Transform getters
    glm::vec3 GetPosition() const { return m_Position; }
    glm::vec3 GetRotation() const { return m_Rotation; }
    glm::vec3 GetScale() const { return m_Scale; }

    // Get model matrix for rendering
    glm::mat4 GetModelMatrix() const;

    // Getters
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<unsigned int>& GetIndices() const { return m_Indices; }

    VertexArray* GetVAO() const { return m_VAO.get(); }
    IndexBuffer* GetIBO() const { return m_IBO.get(); }
    Texture* GetTexture() const { return m_Texture.get(); }

    glm::vec3 GetBoundsMin() const { return m_BoundsMin; }
    glm::vec3 GetBoundsMax() const { return m_BoundsMax; }
    glm::vec3 GetBoundsCenter() const { return (m_BoundsMin + m_BoundsMax) * 0.5f; }
    glm::vec3 GetBoundsSize() const { return m_BoundsMax - m_BoundsMin; }

    // Info
    size_t GetVertexCount() const { return m_Vertices.size(); }
    size_t GetIndexCount() const { return m_Indices.size(); }
    size_t GetTriangleCount() const { return m_Indices.size() / 3; }

    bool HasMesh() const { return !m_Vertices.empty(); }
    bool IsUploadedToGPU() const { return m_VAO != nullptr; }

private:
    void CalculateBounds();
};
