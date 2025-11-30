#pragma once

#include "Chunk.h"
#include "Voxel.h"
#include <vector>
#include <memory>

// Forward declarations
class VertexArray;
class VertexBuffer;
class IndexBuffer;
class VertexBufferLayout;

// Vertex structure for voxel rendering
struct VoxelVertex
{
    float position[3];      // X, Y, Z position
    float texCoords[2];     // U, V texture coordinates
    float normal[3];        // Normal vector for lighting
    float ambientOcclusion; // Ambient occlusion value (0.0 - 1.0)

    VoxelVertex()
        : position{ 0.0f, 0.0f, 0.0f }
        , texCoords{ 0.0f, 0.0f }
        , normal{ 0.0f, 0.0f, 0.0f }
        , ambientOcclusion(1.0f)
    {
    }

    VoxelVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz, float ao = 1.0f)
        : position{ x, y, z }
        , texCoords{ u, v }
        , normal{ nx, ny, nz }
        , ambientOcclusion(ao)
    {
    }
};

// Face data for mesh building
struct FaceData
{
    int x, y, z;              // Voxel position
    VoxelFace face;           // Which face
    uint16_t textureIndex;    // Texture atlas index

    FaceData(int x, int y, int z, VoxelFace face, uint16_t texIndex)
        : x(x), y(y), z(z), face(face), textureIndex(texIndex)
    {
    }
};

// Mesh builder for chunks
class ChunkMeshBuilder
{
private:
    Chunk* m_Chunk;

    std::vector<VoxelVertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    unsigned int m_CurrentVertexCount;

    // Texture atlas parameters (set these based on your texture atlas)
    static constexpr int ATLAS_SIZE = 256;      // Atlas texture size in pixels
    static constexpr int TEXTURE_SIZE = 16;     // Individual texture size in pixels
    static constexpr int TEXTURES_PER_ROW = ATLAS_SIZE / TEXTURE_SIZE; // 16 textures per row

public:
    explicit ChunkMeshBuilder(Chunk* chunk);
    ~ChunkMeshBuilder();

    // Build the mesh for the chunk
    void BuildMesh();

    // Build mesh with greedy meshing optimization
    void BuildMeshGreedy();

    // Apply the built mesh to the chunk
    void ApplyMeshToChunk();

    // Get mesh statistics
    int GetVertexCount() const { return static_cast<int>(m_Vertices.size()); }
    int GetIndexCount() const { return static_cast<int>(m_Indices.size()); }
    int GetTriangleCount() const { return GetIndexCount() / 3; }

private:
    // Clear current mesh data
    void Clear();

    // Add a face to the mesh
    void AddFace(int x, int y, int z, VoxelFace face, uint16_t textureIndex);

    // Add a quad (4 vertices, 6 indices for 2 triangles)
    void AddQuad(const VoxelVertex& v0, const VoxelVertex& v1,
        const VoxelVertex& v2, const VoxelVertex& v3);

    // Calculate UV coordinates from texture atlas index
    void GetTextureCoords(uint16_t textureIndex, float uvCoords[8]);

    // Calculate ambient occlusion for a vertex
    float CalculateAO(int x, int y, int z, VoxelFace face, int corner);

    // Check if a voxel blocks light (for AO calculation)
    bool IsOccluding(int x, int y, int z) const;

    // Get face normal vector
    void GetFaceNormal(VoxelFace face, float normal[3]);

    // Get face vertices in world space
    void GetFaceVertices(int x, int y, int z, VoxelFace face, float vertices[12]);
};