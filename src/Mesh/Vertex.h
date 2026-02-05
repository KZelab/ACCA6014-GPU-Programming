#pragma once

struct Vertex {
    float position[3];
    float normal[3];
    float colour[3];
    float texCoords[2];

    Vertex(float px, float py, float pz, float nx, float ny, float nz, float r, float g, float b, float u, float v)
        : position{ px, py, pz }, normal{ nx, ny, nz }, colour{ r, g, b }, texCoords{ u, v } {
    }

    // Convenience constructor for backward compatibility (assumes normal pointing up)
    Vertex(float px, float py, float pz, float r, float g, float b, float u, float v)
        : position{ px, py, pz }, normal{ 0.0f, 0.0f, 1.0f }, colour{ r, g, b }, texCoords{ u, v } {
    }
};
