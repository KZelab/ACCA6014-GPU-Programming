#pragma once

#include "Mesh.h"
#include <vector>

class GeometryFactory {
public:
    // Basic primitive generators
    static Mesh* CreateTriangle();
    static Mesh* CreateQuad();
    static Mesh* CreateCube();
    static Mesh* CreateSphere(int sectors = 20, int stacks = 20);
    static Mesh* CreateFullscreenQuad();

    // Advanced primitive generators
    static Mesh* CreateCylinder(int sectors = 20, int stacks = 1, float height = 1.0f, float radius = 0.5f);
    static Mesh* CreatePlane(int subdivisionsX = 1, int subdivisionsY = 1, float width = 1.0f, float height = 1.0f);

    // Utility methods for vertex data management
    static std::vector<Vertex> GenerateTriangleVertices();
    static std::vector<Vertex> GenerateQuadVertices();
    static std::vector<Vertex> GenerateCubeVertices();
    static std::vector<Vertex> GenerateSphereVertices(int sectors = 20, int stacks = 20);
    static std::vector<Vertex> GenerateFullscreenQuadVertices();
    static std::vector<Vertex> GenerateCylinderVertices(int sectors = 20, int stacks = 1, float height = 1.0f, float radius = 0.5f);
    static std::vector<Vertex> GeneratePlaneVertices(int subdivisionsX = 1, int subdivisionsY = 1, float width = 1.0f, float height = 1.0f);

    // Index buffer generation
    static std::vector<unsigned int> GenerateTriangleIndices();
    static std::vector<unsigned int> GenerateQuadIndices();
    static std::vector<unsigned int> GenerateCubeIndices();
    static std::vector<unsigned int> GenerateSphereIndices(int sectors = 20, int stacks = 20);
    static std::vector<unsigned int> GenerateFullscreenQuadIndices();
    static std::vector<unsigned int> GenerateCylinderIndices(int sectors = 20, int stacks = 1);
    static std::vector<unsigned int> GeneratePlaneIndices(int subdivisionsX = 1, int subdivisionsY = 1);

private:
    // Helper methods for UV coordinate calculation
    static void AssignUVCoordinates(std::vector<Vertex>& vertices, int faceVertexCount);
    static void AssignColors(std::vector<Vertex>& vertices);
    static void CalculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};