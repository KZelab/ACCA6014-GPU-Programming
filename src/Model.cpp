#include <GL/glew.h>  // MUST be included before any OpenGL headers

#include "Model.h"
#include "glm/gtc/matrix_transform.hpp"
#include <algorithm>
#include <limits>
#include <iostream>

Model::Model()
    : m_Position(0.0f)
    , m_Rotation(0.0f)
    , m_Scale(1.0f)
    , m_BoundsMin(0.0f)
    , m_BoundsMax(0.0f)
{
}

Model::~Model()
{
}

void Model::SetMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    m_Vertices = vertices;
    m_Indices = indices;
    CalculateBounds();
}

void Model::CenterModelAtBase()
{
    if (m_Vertices.empty())
        return;

    // Calculate center offset (center X and Z, but keep feet at Y=0)
    glm::vec3 boundsCenter = GetBoundsCenter();
    glm::vec3 offset(boundsCenter.x, m_BoundsMin.y, boundsCenter.z);

    std::cout << "Centering model - Original bounds center: (" << boundsCenter.x << ", " << boundsCenter.y << ", " << boundsCenter.z << ")" << std::endl;
    std::cout << "                   Offset: (" << offset.x << ", " << offset.y << ", " << offset.z << ")" << std::endl;

    // Shift all vertices to center the model
    for (auto& vertex : m_Vertices)
    {
        vertex.position -= offset;
    }

    // Recalculate bounds after centering
    CalculateBounds();

    std::cout << "Model centered! New bounds: (" << m_BoundsMin.x << ", " << m_BoundsMin.y << ", " << m_BoundsMin.z << ") to ("
              << m_BoundsMax.x << ", " << m_BoundsMax.y << ", " << m_BoundsMax.z << ")" << std::endl;
}

void Model::UploadToGPU()
{
    if (m_Vertices.empty() || m_Indices.empty())
    {
        return;
    }

    // Create vertex array
    m_VAO = std::make_unique<VertexArray>();

    // Create vertex buffer with interleaved data
    m_VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));

    // Set up vertex layout
    VertexBufferLayout layout;
    layout.Push<float>(3);  // Position (vec3)
    layout.Push<float>(3);  // Normal (vec3)
    layout.Push<float>(2);  // TexCoord (vec2)

    m_VAO->AddBuffer(*m_VBO, layout);

    // Create index buffer
    m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
}

glm::mat4 Model::GetModelMatrix() const
{
    // Build transformation matrix using individual matrices
    // Order: Translate * Rotate * Scale (TRS)
    // This applies to vertices as: Scale first, then Rotate, then Translate

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), m_Position);

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    rotation = glm::rotate(rotation, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    rotation = glm::rotate(rotation, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), m_Scale);

    // Combine: T * R * S
    return translation * rotation * scale;
}

void Model::CalculateBounds()
{
    if (m_Vertices.empty())
    {
        m_BoundsMin = glm::vec3(0.0f);
        m_BoundsMax = glm::vec3(0.0f);
        return;
    }

    // Initialize with first vertex
    m_BoundsMin = m_Vertices[0].position;
    m_BoundsMax = m_Vertices[0].position;

    // Find min and max for each axis
    for (const auto& vertex : m_Vertices)
    {
        m_BoundsMin.x = std::min(m_BoundsMin.x, vertex.position.x);
        m_BoundsMin.y = std::min(m_BoundsMin.y, vertex.position.y);
        m_BoundsMin.z = std::min(m_BoundsMin.z, vertex.position.z);

        m_BoundsMax.x = std::max(m_BoundsMax.x, vertex.position.x);
        m_BoundsMax.y = std::max(m_BoundsMax.y, vertex.position.y);
        m_BoundsMax.z = std::max(m_BoundsMax.z, vertex.position.z);
    }
}

bool Model::LoadTexture(const std::string& filepath)
{
    std::cout << "Loading model texture: " << filepath << std::endl;

    m_Texture = std::make_unique<Texture>(filepath);

    if (m_Texture->getRendererID() == 0)
    {
        std::cerr << "ERROR: Failed to load texture: " << filepath << std::endl;
        m_Texture.reset();
        return false;
    }

    std::cout << "Texture loaded successfully!" << std::endl;
    return true;
}
