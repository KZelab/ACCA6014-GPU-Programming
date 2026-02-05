#pragma once

#include "Mesh.h"
#include <vector>

class GeometryFactory {
public:
    // Basic primitive generators
    static std::unique_ptr<Mesh> CreateTriangle();
    static std::unique_ptr<Mesh> CreateQuad();
    static std::unique_ptr<Mesh> CreateCube();
    static std::unique_ptr<Mesh> CreateSphere(int sectors = 20, int stacks = 20);
    static std::unique_ptr<Mesh> CreateFullscreenQuad();

    // Utility methods for vertex data management
    static std::vector<Vertex> GenerateTriangleVertices();
    static std::vector<Vertex> GenerateQuadVertices();
    static std::vector<Vertex> GenerateCubeVertices();
    static std::vector<Vertex> GenerateSphereVertices(int sectors = 20, int stacks = 20);
    static std::vector<Vertex> GenerateFullscreenQuadVertices();

    // Index buffer generation
    static std::vector<unsigned int> GenerateTriangleIndices();
    static std::vector<unsigned int> GenerateQuadIndices();
    static std::vector<unsigned int> GenerateCubeIndices();
    static std::vector<unsigned int> GenerateSphereIndices(int sectors = 20, int stacks = 20);
    static std::vector<unsigned int> GenerateFullscreenQuadIndices();

private:
    // Helper methods for UV coordinate calculation
    static void AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount);
    static void AssignColors(std::vector<Vertex>& vertices);
    static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};