#include "FullscreenQuad.h"

FullscreenQuad::FullscreenQuad()
{
    // Full-screen quad in NDC (No transformation needed)
    m_Vertices = {
        // Positions         // Normals           // TexCoords
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // Bottom-right
        -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // Top-left
         1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f   // Top-right
    };

    m_Indices = {
        0, 1, 2,  // First triangle
        1, 3, 2   // Second triangle
    };

	SetupMesh();
}
