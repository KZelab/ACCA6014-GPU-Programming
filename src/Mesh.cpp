#include "Mesh.h"
#include <iostream>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_Vertices(vertices), m_Indices(indices), m_IndexCount(indices.size())
{
    SetupMesh();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::Bind() const
{
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind() const
{
    glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
    // Generate and bind VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Generate and bind VBO
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // Colour attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colour));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
}

Mesh* Mesh::CreateCube()
{
    std::vector<Vertex> vertices = {
        // Front face vertices
        Vertex(-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // 0: Front-bottom-left - Red
        Vertex( 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // 1: Front-bottom-right - Green
        Vertex( 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // 2: Front-top-right - Blue
        Vertex(-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f),  // 3: Front-top-left - Yellow

        // Back face vertices
        Vertex(-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f),  // 4: Back-bottom-left - Magenta
        Vertex( 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // 5: Back-bottom-right - Cyan
        Vertex( 0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,  0.0f, 1.0f),  // 6: Back-top-right - Orange
        Vertex(-0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f)   // 7: Back-top-left - Grey
    };

    std::vector<unsigned int> indices = {
        // Front face (Z = +0.5)
        0, 1, 2,  2, 3, 0,
        // Back face (Z = -0.5)
        4, 5, 6,  6, 7, 4,
        // Left face (X = -0.5)
        7, 3, 0,  0, 4, 7,
        // Right face (X = +0.5)
        1, 5, 6,  6, 2, 1,
        // Bottom face (Y = -0.5)
        4, 0, 1,  1, 5, 4,
        // Top face (Y = +0.5)
        3, 7, 6,  6, 2, 3
    };

    return new Mesh(vertices, indices);
}