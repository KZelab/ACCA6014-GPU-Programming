#include "Sphere.h"
#include <cmath>
#define M_PI 3.14
Sphere::Sphere(int sectors, int stacks) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    float radius = 0.5f;
    for (int stack = 0; stack <= stacks; ++stack) {
        float phi = M_PI * (float(stack) / stacks); // from 0 to π
        float y = cos(phi);
        float sinPhi = sin(phi);

        for (int sector = 0; sector <= sectors; ++sector) {
            float theta = 2.0f * M_PI * (float(sector) / sectors); // from 0 to 2π
            float x = sinPhi * cos(theta);
            float z = sinPhi * sin(theta);
            float u = float(sector) / sectors;
            float v = float(stack) / stacks;

            vertices.push_back(radius * x);  // X position
            vertices.push_back(radius * y);  // Y position
            vertices.push_back(radius * z);  // Z position
            vertices.push_back(x);           // Normal X
            vertices.push_back(y);           // Normal Y
            vertices.push_back(z);           // Normal Z
            vertices.push_back(u);           // Texture U
            vertices.push_back(v);           // Texture V
        }
    }

    // Generate indices
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

    loadVertices(vertices, indices);
    SetupMesh();
}