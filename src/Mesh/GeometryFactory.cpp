#include "GeometryFactory.h"
#include <cmath>

Mesh* GeometryFactory::CreateTriangle() {
    std::vector<Vertex> vertices = GenerateTriangleVertices();
    std::vector<unsigned int> indices = GenerateTriangleIndices();
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreateQuad() {
    std::vector<Vertex> vertices = GenerateQuadVertices();
    std::vector<unsigned int> indices = GenerateQuadIndices();
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreateCube() {
    std::vector<Vertex> vertices = GenerateCubeVertices();
    std::vector<unsigned int> indices = GenerateCubeIndices();
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreateSphere(int sectors, int stacks) {
    std::vector<Vertex> vertices = GenerateSphereVertices(sectors, stacks);
    std::vector<unsigned int> indices = GenerateSphereIndices(sectors, stacks);
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreateFullscreenQuad() {
    std::vector<Vertex> vertices = GenerateFullscreenQuadVertices();
    std::vector<unsigned int> indices = GenerateFullscreenQuadIndices();
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreateCylinder(int sectors, int stacks, float height, float radius) {
    std::vector<Vertex> vertices = GenerateCylinderVertices(sectors, stacks, height, radius);
    std::vector<unsigned int> indices = GenerateCylinderIndices(sectors, stacks);
    return new Mesh(vertices, indices);
}

Mesh* GeometryFactory::CreatePlane(int subdivisionsX, int subdivisionsY, float width, float height) {
    std::vector<Vertex> vertices = GeneratePlaneVertices(subdivisionsX, subdivisionsY, width, height);
    std::vector<unsigned int> indices = GeneratePlaneIndices(subdivisionsX, subdivisionsY);
    return new Mesh(vertices, indices);
}

std::vector<Vertex> GeometryFactory::GenerateTriangleVertices() {
    std::vector<Vertex> vertices = {
        Vertex( 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.5f, 1.0f),  // Top vertex - Red
        Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Green
        Vertex( 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f)   // Bottom-right - Blue
    };
    return vertices;
}

std::vector<Vertex> GeometryFactory::GenerateQuadVertices() {
    std::vector<Vertex> vertices = {
        Vertex(-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // Bottom-left - Red
        Vertex( 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // Bottom-right - Green
        Vertex( 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // Top-right - Blue
        Vertex(-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f)   // Top-left - Yellow
    };
    return vertices;
}

std::vector<Vertex> GeometryFactory::GenerateCubeVertices() {
    std::vector<Vertex> vertices = {
        // Front face vertices (Z = +0.5) - Normal pointing towards +Z
        Vertex(-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f),  // 0: Front-bottom-left - Red
        Vertex( 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f),  // 1: Front-bottom-right - Green
        Vertex( 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f),  // 2: Front-top-right - Blue
        Vertex(-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f),  // 3: Front-top-left - Yellow

        // Back face vertices (Z = -0.5) - Normal pointing towards -Z
        Vertex(-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f),  // 4: Back-bottom-left - Magenta
        Vertex( 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // 5: Back-bottom-right - Cyan
        Vertex( 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.5f, 0.0f,  0.0f, 1.0f),  // 6: Back-top-right - Orange
        Vertex(-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f)   // 7: Back-top-left - Grey
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateTriangleIndices() {
    return { 0, 1, 2 };
}

std::vector<unsigned int> GeometryFactory::GenerateQuadIndices() {
    return { 0, 1, 2, 2, 3, 0 };
}

std::vector<unsigned int> GeometryFactory::GenerateCubeIndices() {
    return {
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
}

void GeometryFactory::AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (faceVertexCount == 3) {
            // Triangle UV mapping
            switch (i % 3) {
                case 0: vertices[i].texCoords[0] = 0.5f; vertices[i].texCoords[1] = 1.0f; break;
                case 1: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 0.0f; break;
                case 2: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 0.0f; break;
            }
        } else if (faceVertexCount == 4) {
            // Quad UV mapping
            switch (i % 4) {
                case 0: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 0.0f; break;
                case 1: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 0.0f; break;
                case 2: vertices[i].texCoords[0] = 1.0f; vertices[i].texCoords[1] = 1.0f; break;
                case 3: vertices[i].texCoords[0] = 0.0f; vertices[i].texCoords[1] = 1.0f; break;
            }
        }
    }
}

std::vector<Vertex> GeometryFactory::GenerateSphereVertices(int sectors, int stacks) {
    std::vector<Vertex> vertices;
    float radius = 0.5f;
    const float PI = 3.14159265359f;

    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = PI * (float(stack) / stacks); // from 0 to π
        float y = cos(phi);
        float sinPhi = sin(phi);

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * PI * (float(sector) / sectors); // from 0 to 2π
            float x = sinPhi * cos(theta);
            float z = sinPhi * sin(theta);
            float u = float(sector) / sectors;
            float v = float(stack) / stacks;

            // Position
            float posX = radius * x;
            float posY = radius * y;
            float posZ = radius * z;

            // Normal (for unit sphere, normal = normalized position)
            float normX = x;
            float normY = y;
            float normZ = z;

            // Color based on position for visual variety
            float r = (x + 1.0f) * 0.5f;
            float g = (y + 1.0f) * 0.5f;
            float b = (z + 1.0f) * 0.5f;

            vertices.emplace_back(posX, posY, posZ, normX, normY, normZ, r, g, b, u, v);
        }
    }

    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateSphereIndices(int sectors, int stacks) {
    std::vector<unsigned int> indices;

    for (int stack = 0; stack < stacks; ++stack) {
        int k1 = stack * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int sector = 0; sector < sectors; ++sector, ++k1, ++k2) {
            if (stack != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (stack != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    return indices;
}

std::vector<Vertex> GeometryFactory::GenerateFullscreenQuadVertices() {
    std::vector<Vertex> vertices = {
        // NDC coordinates for fullscreen rendering
        Vertex(-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f),  // Bottom-left - White
        Vertex( 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f),  // Bottom-right - White
        Vertex(-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f),  // Top-left - White
        Vertex( 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f)   // Top-right - White
    };
    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateFullscreenQuadIndices() {
    return { 0, 1, 2, 1, 3, 2 };
}

void GeometryFactory::AssignColors(std::vector<Vertex>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        float hue = (float)i / vertices.size() * 360.0f;

        float c = 1.0f;
        float x = c * (1.0f - std::abs(std::fmod(hue / 60.0f, 2.0f) - 1.0f));
        float m = 0.0f;

        float r, g, b;
        if (hue < 60.0f) { r = c; g = x; b = 0; }
        else if (hue < 120.0f) { r = x; g = c; b = 0; }
        else if (hue < 180.0f) { r = 0; g = c; b = x; }
        else if (hue < 240.0f) { r = 0; g = x; b = c; }
        else if (hue < 300.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        vertices[i].colour[0] = r + m;
        vertices[i].colour[1] = g + m;
        vertices[i].colour[2] = b + m;
    }
}

void GeometryFactory::CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Reset all normals to zero
    for (auto& vertex : vertices) {
        vertex.normal[0] = vertex.normal[1] = vertex.normal[2] = 0.0f;
    }

    // Calculate face normals and accumulate
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Get vertex positions
        float v1[3] = {vertices[i1].position[0] - vertices[i0].position[0],
                       vertices[i1].position[1] - vertices[i0].position[1],
                       vertices[i1].position[2] - vertices[i0].position[2]};

        float v2[3] = {vertices[i2].position[0] - vertices[i0].position[0],
                       vertices[i2].position[1] - vertices[i0].position[1],
                       vertices[i2].position[2] - vertices[i0].position[2]};

        // Cross product for face normal
        float normal[3] = {v1[1] * v2[2] - v1[2] * v2[1],
                           v1[2] * v2[0] - v1[0] * v2[2],
                           v1[0] * v2[1] - v1[1] * v2[0]};

        // Add to vertex normals
        for (int j = 0; j < 3; j++) {
            vertices[i0].normal[j] += normal[j];
            vertices[i1].normal[j] += normal[j];
            vertices[i2].normal[j] += normal[j];
        }
    }

    // Normalize all vertex normals
    for (auto& vertex : vertices) {
        float length = sqrt(vertex.normal[0] * vertex.normal[0] +
                           vertex.normal[1] * vertex.normal[1] +
                           vertex.normal[2] * vertex.normal[2]);
        if (length > 0.0f) {
            vertex.normal[0] /= length;
            vertex.normal[1] /= length;
            vertex.normal[2] /= length;
        }
    }
}

std::vector<Vertex> GeometryFactory::GenerateCylinderVertices(int sectors, int stacks, float height, float radius) {
    std::vector<Vertex> vertices;
    const float PI = 3.14159265359f;

    // Generate vertices for the cylinder body
    for (int stack = 0; stack <= stacks; ++stack) {
        float y = height * (float(stack) / stacks) - height * 0.5f; // Center cylinder vertically

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * PI * (float(sector) / sectors);
            float x = radius * cos(theta);
            float z = radius * sin(theta);

            // Normal (pointing outward from cylinder axis)
            float normX = cos(theta);
            float normY = 0.0f;
            float normZ = sin(theta);

            // Texture coordinates
            float u = float(sector) / sectors;
            float v = float(stack) / stacks;

            // Colour based on height and angle
            float r = 0.5f + 0.5f * sin(theta);
            float g = 0.5f + 0.5f * (y / height + 0.5f);
            float b = 0.5f + 0.5f * cos(theta);

            vertices.emplace_back(x, y, z, normX, normY, normZ, r, g, b, u, v);
        }
    }

    return vertices;
}

std::vector<unsigned int> GeometryFactory::GenerateCylinderIndices(int sectors, int stacks) {
    std::vector<unsigned int> indices;

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

    return indices;
}

std::vector<Vertex> GeometryFactory::GeneratePlaneVertices(int subdivisionsX, int subdivisionsY, float width, float height) {
    std::vector<Vertex> vertices;

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

    return vertices;
}

std::vector<unsigned int> GeometryFactory::GeneratePlaneIndices(int subdivisionsX, int subdivisionsY) {
    std::vector<unsigned int> indices;
    int verticesX = subdivisionsX + 1;

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

    return indices;
}