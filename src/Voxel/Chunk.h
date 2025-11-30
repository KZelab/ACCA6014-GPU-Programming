#pragma once

#include "Voxel.h"
#include <array>
#include <memory>

// Chunk dimensions - 16x16x16 is standard for Minecraft-like games
// Can be adjusted for performance vs detail trade-off
constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 256; // World height
constexpr int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT;

// Forward declarations
class VertexArray;
class VertexBuffer;
class IndexBuffer;

// Represents a chunk's position in the world
struct ChunkPosition
{
    int x;
    int z;

    ChunkPosition() : x(0), z(0) {}
    ChunkPosition(int x, int z) : x(x), z(z) {}

    // Comparison operators for use in maps/sets
    bool operator==(const ChunkPosition& other) const
    {
        return x == other.x && z == other.z;
    }

    bool operator!=(const ChunkPosition& other) const
    {
        return !(*this == other);
    }

    bool operator<(const ChunkPosition& other) const
    {
        if (x != other.x) return x < other.x;
        return z < other.z;
    }
};

// Hash function for ChunkPosition to use in unordered_map
namespace std
{
    template<>
    struct hash<ChunkPosition>
    {
        size_t operator()(const ChunkPosition& pos) const
        {
            // Combine the two integers into a single hash
            return hash<int>()(pos.x) ^ (hash<int>()(pos.z) << 1);
        }
    };
}

// A chunk is a 3D section of the world containing voxels
class Chunk
{
private:
    // Chunk position in world chunk coordinates
    ChunkPosition m_Position;

    // 3D array of voxels stored as 1D array
    // Layout: x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE
    std::array<Voxel, CHUNK_VOLUME> m_Voxels;

    // Mesh data
    std::unique_ptr<VertexArray> m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer> m_IBO;

    // Flags
    bool m_IsDirty;        // Does mesh need rebuilding?
    bool m_IsEmpty;        // Contains only air?
    bool m_HasMesh;        // Has mesh been generated?

    // Neighbour chunks (for face culling across chunk boundaries)
    Chunk* m_NeighbourNorth;  // +Z
    Chunk* m_NeighbourSouth;  // -Z
    Chunk* m_NeighbourEast;   // +X
    Chunk* m_NeighbourWest;   // -X

    // Statistics
    int m_VertexCount;
    int m_IndexCount;

public:
    // Constructor
    explicit Chunk(const ChunkPosition& position);
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    // Voxel access
    // Local coordinates (0 to CHUNK_SIZE-1 for x/z, 0 to CHUNK_HEIGHT-1 for y)
    Voxel& GetVoxel(int x, int y, int z);
    const Voxel& GetVoxel(int x, int y, int z) const;

    void SetVoxel(int x, int y, int z, const Voxel& voxel);
    void SetVoxel(int x, int y, int z, VoxelType type);

    // Check if coordinates are within chunk bounds
    static bool IsValidPosition(int x, int y, int z);

    // Convert 3D coordinates to 1D array index
    static int GetVoxelIndex(int x, int y, int z);

    // Position
    ChunkPosition GetPosition() const { return m_Position; }
    int GetChunkX() const { return m_Position.x; }
    int GetChunkZ() const { return m_Position.z; }

    // World position (in voxel coordinates)
    int GetWorldX() const { return m_Position.x * CHUNK_SIZE; }
    int GetWorldZ() const { return m_Position.z * CHUNK_SIZE; }

    // Neighbour management
    void SetNeighbour(VoxelFace direction, Chunk* chunk);
    Chunk* GetNeighbour(VoxelFace direction) const;

    // Check if neighbour exists
    bool HasNeighbour(VoxelFace direction) const;

    // Mesh management
    bool IsDirty() const { return m_IsDirty; }
    void SetDirty(bool dirty) { m_IsDirty = dirty; }

    bool IsEmpty() const { return m_IsEmpty; }
    void UpdateEmptyStatus();

    bool HasMesh() const { return m_HasMesh; }

    // Mesh data access (for rendering)
    VertexArray* GetVAO() const { return m_VAO.get(); }
    IndexBuffer* GetIBO() const { return m_IBO.get(); }

    int GetVertexCount() const { return m_VertexCount; }
    int GetIndexCount() const { return m_IndexCount; }

    // Set mesh data (called by ChunkMeshBuilder)
    void SetMeshData(std::unique_ptr<VertexArray> vao,
        std::unique_ptr<VertexBuffer> vbo,
        std::unique_ptr<IndexBuffer> ibo,
        int vertexCount,
        int indexCount);

    // Clear mesh data (to free memory)
    void ClearMesh();

    // Utility functions

    // Fill entire chunk with a single voxel type (useful for testing)
    void Fill(VoxelType type);

    // Fill a layer at height y with a voxel type
    void FillLayer(int y, VoxelType type);

    // Fill a range of layers
    void FillLayers(int yStart, int yEnd, VoxelType type);

    // Clear all voxels to air
    void Clear();

    // Check if a voxel face is exposed (for culling)
    bool IsFaceVisible(int x, int y, int z, VoxelFace face) const;
};