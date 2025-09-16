#include "Plane.h"

Plane::Plane(int subdivisionsX, int subdivisionsY, float width, float height) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Calculate number of vertices
    int verticesX = subdivisionsX + 1;
    int verticesY = subdivisionsY + 1;

    // Generate vertices with subdivision
    for (int y = 0; y < verticesY; ++y) {
        for (int x = 0; x < verticesX; ++x) {
            // Calculate position
            float posX = (float(x) / subdivisionsX - 0.5f) * width;
            float posY = 0.0f; // Plane lies in XZ plane
            float posZ = (float(y) / subdivisionsY - 0.5f) * height;

            // Normal pointing up
            float normX = 0.0f;
            float normY = 1.0f;
            float normZ = 0.0f;

            // UV coordinates
            float u = float(x) / subdivisionsX;
            float v = float(y) / subdivisionsY;

            // Colour based on position for visual distinction
            float r = 0.5f + 0.5f * (posX / width + 0.5f);
            float g = 0.7f;
            float b = 0.5f + 0.5f * (posZ / height + 0.5f);

            vertices.emplace_back(posX, posY, posZ, normX, normY, normZ, r, g, b, u, v);
        }
    }

    // Generate indices for tessellated plane
    for (int y = 0; y < subdivisionsY; ++y) {
        for (int x = 0; x < subdivisionsX; ++x) {
            // Calculate indices for current quad
            int topLeft = y * verticesX + x;
            int topRight = topLeft + 1;
            int bottomLeft = (y + 1) * verticesX + x;
            int bottomRight = bottomLeft + 1;

            // First triangle (top-left, bottom-left, top-right)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            // Second triangle (top-right, bottom-left, bottom-right)
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    m_Vertices = vertices;
    m_Indices = indices;
    m_IndexCount = indices.size();

    SetupMesh();
}