#pragma once

#include <GL/glew.h>
#include <vector>

struct Vertex {
    float position[3];
    float normal[3];
    float colour[3];
    float texCoords[2];

    Vertex(float px, float py, float pz, float nx, float ny, float nz, float r, float g, float b, float u, float v)
        : position{px, py, pz}, normal{nx, ny, nz}, colour{r, g, b}, texCoords{u, v} {}

    // Convenience constructor for backward compatibility (assumes normal pointing up)
    Vertex(float px, float py, float pz, float r, float g, float b, float u, float v)
        : position{px, py, pz}, normal{0.0f, 0.0f, 1.0f}, colour{r, g, b}, texCoords{u, v} {}
};

class Mesh {
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    unsigned int m_IndexCount;

public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void Bind() const;
    void Unbind() const;

    unsigned int GetIndexCount() const { return m_IndexCount; }



private:
    void SetupMesh();
};