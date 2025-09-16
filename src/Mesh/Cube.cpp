#include "Cube.h"

Cube::Cube() {
    m_Vertices = {
        // Positions          // Normals         // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  // Back face
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  // Front face
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f
    };

    m_Indices = {
        0, 1, 2, 2, 3, 0,   // Back face
        4, 5, 6, 6, 7, 4,   // Front face
        0, 3, 7, 7, 4, 0,   // Left face
        1, 2, 6, 6, 5, 1,   // Right face
        3, 2, 6, 6, 7, 3,   // Top face
        0, 1, 5, 5, 4, 0    // Bottom face
    };

    SetupMesh();
}
