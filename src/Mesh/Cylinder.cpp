#include "Cylinder.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359f
#endif

Cylinder::Cylinder(int sectors, int stacks, float height, float radius) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices for the cylinder body
    for (int stack = 0; stack <= stacks; ++stack) {
        float y = height * (float(stack) / stacks) - height * 0.5f; // Center cylinder vertically

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * M_PI * (float(sector) / sectors);
            float x = radius * cos(theta);
            float z = radius * sin(theta);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal (pointing outward from cylinder axis)
            vertices.push_back(cos(theta));
            vertices.push_back(0.0f);
            vertices.push_back(sin(theta));

            // Texture coordinates
            float u = float(sector) / sectors;
            float v = float(stack) / stacks;
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Add center vertices for top and bottom caps
    int bodyVertexCount = (stacks + 1) * (sectors + 1);

    // Bottom cap center
    vertices.push_back(0.0f);                    // x
    vertices.push_back(-height * 0.5f);          // y
    vertices.push_back(0.0f);                    // z
    vertices.push_back(0.0f);                    // normal x
    vertices.push_back(-1.0f);                   // normal y (pointing down)
    vertices.push_back(0.0f);                    // normal z
    vertices.push_back(0.5f);                    // u (center of texture)
    vertices.push_back(0.5f);                    // v (center of texture)

    // Top cap center
    vertices.push_back(0.0f);                    // x
    vertices.push_back(height * 0.5f);           // y
    vertices.push_back(0.0f);                    // z
    vertices.push_back(0.0f);                    // normal x
    vertices.push_back(1.0f);                    // normal y (pointing up)
    vertices.push_back(0.0f);                    // normal z
    vertices.push_back(0.5f);                    // u (center of texture)
    vertices.push_back(0.5f);                    // v (center of texture)

    // Add vertices for cap edges
    // Bottom cap edge vertices
    for (int sector = 0; sector <= sectors; ++sector) {
        float theta = 2.0f * M_PI * (float(sector) / sectors);
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(x);                   // x
        vertices.push_back(-height * 0.5f);     // y
        vertices.push_back(z);                   // z
        vertices.push_back(0.0f);                // normal x
        vertices.push_back(-1.0f);               // normal y (pointing down)
        vertices.push_back(0.0f);                // normal z
        vertices.push_back((cos(theta) + 1.0f) * 0.5f); // u
        vertices.push_back((sin(theta) + 1.0f) * 0.5f); // v
    }

    // Top cap edge vertices
    for (int sector = 0; sector <= sectors; ++sector) {
        float theta = 2.0f * M_PI * (float(sector) / sectors);
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        vertices.push_back(x);                   // x
        vertices.push_back(height * 0.5f);      // y
        vertices.push_back(z);                   // z
        vertices.push_back(0.0f);                // normal x
        vertices.push_back(1.0f);                // normal y (pointing up)
        vertices.push_back(0.0f);                // normal z
        vertices.push_back((cos(theta) + 1.0f) * 0.5f); // u
        vertices.push_back((sin(theta) + 1.0f) * 0.5f); // v
    }

    // Generate indices for cylinder body
    for (int stack = 0; stack < stacks; ++stack) {
        int k1 = stack * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int sector = 0; sector < sectors; ++sector, ++k1, ++k2) {
            // Two triangles per quad
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }

    // Generate indices for caps
    int bottomCenterIndex = bodyVertexCount;
    int topCenterIndex = bodyVertexCount + 1;
    int bottomCapStartIndex = bodyVertexCount + 2;
    int topCapStartIndex = bottomCapStartIndex + sectors + 1;

    // Bottom cap triangles
    for (int sector = 0; sector < sectors; ++sector) {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCapStartIndex + sector + 1);
        indices.push_back(bottomCapStartIndex + sector);
    }

    // Top cap triangles
    for (int sector = 0; sector < sectors; ++sector) {
        indices.push_back(topCenterIndex);
        indices.push_back(topCapStartIndex + sector);
        indices.push_back(topCapStartIndex + sector + 1);
    }

    // Convert to Vertex objects and set up mesh
    std::vector<Vertex> vertexObjects;
    for (size_t i = 0; i < vertices.size(); i += 8) {
        float r = 0.6f + 0.4f * sin(vertices[i] + vertices[i+1] + vertices[i+2]); // Procedural colour
        float g = 0.6f + 0.4f * cos(vertices[i] * 2.0f);
        float b = 0.6f + 0.4f * sin(vertices[i+2] * 3.0f);

        vertexObjects.emplace_back(
            vertices[i], vertices[i+1], vertices[i+2],       // position
            vertices[i+3], vertices[i+4], vertices[i+5],     // normal
            r, g, b,                                         // colour
            vertices[i+6], vertices[i+7]                     // texture coordinates
        );
    }

    m_Vertices = vertexObjects;
    m_Indices = indices;
    m_IndexCount = indices.size();

    SetupMesh();
}