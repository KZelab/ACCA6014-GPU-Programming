#include "Chunk.h"
#include "../VertexArray.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"

// Constructor with ChunkPosition
Chunk::Chunk(const ChunkPosition& position)
    : m_Position(position)
    , m_IsDirty(true)
    , m_IsEmpty(true)
    , m_HasMesh(false)
    , m_NeighbourNorth(nullptr)
    , m_NeighbourSouth(nullptr)
    , m_NeighbourEast(nullptr)
    , m_NeighbourWest(nullptr)
    , m_VertexCount(0)
    , m_IndexCount(0)
{
    // Initialise all voxels to air
    Clear();
}

// Constructor with separate coordinates
Chunk::Chunk(int chunkX, int chunkZ)
    : Chunk(ChunkPosition(chunkX, chunkZ))
{
}

Chunk::~Chunk()
{
    // Unique pointers will automatically clean up mesh data
}

// Get voxel at local coordinates
Voxel& Chunk::GetVoxel(int x, int y, int z)
{
    return m_Voxels[GetVoxelIndex(x, y, z)];
}

const Voxel& Chunk::GetVoxel(int x, int y, int z) const
{
    return m_Voxels[GetVoxelIndex(x, y, z)];
}

// Set voxel at local coordinates
void Chunk::SetVoxel(int x, int y, int z, const Voxel& voxel)
{

    if (IsValidPosition(x, y, z))
    {
        if (m_Voxels[GetVoxelIndex(x, y, z)].GetType() == voxel.GetType())
            return;

        m_Voxels[GetVoxelIndex(x, y, z)] = voxel;
        m_IsDirty = true; // Mesh needs rebuilding

        if (!voxel.IsAir())
        {
            m_IsEmpty = false;
        }
    }
}

void Chunk::SetVoxel(int x, int y, int z, VoxelType type)
{

    SetVoxel(x, y, z, Voxel(type));
}

// Check if coordinates are within chunk bounds
bool Chunk::IsValidPosition(int x, int y, int z)
{
    return x >= 0 && x < CHUNK_SIZE &&
        y >= 0 && y < CHUNK_HEIGHT &&
        z >= 0 && z < CHUNK_SIZE;
}

// Convert 3D coordinates to 1D array index
// Layout: x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE
int Chunk::GetVoxelIndex(int x, int y, int z)
{
    return x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;
}

// Neighbour management
void Chunk::SetNeighbour(VoxelFace direction, Chunk* chunk)
{
    switch (direction)
    {
    case VoxelFace::FRONT:  // +Z (North)
        m_NeighbourNorth = chunk;
        break;
    case VoxelFace::BACK:   // -Z (South)
        m_NeighbourSouth = chunk;
        break;
    case VoxelFace::RIGHT:  // +X (East)
        m_NeighbourEast = chunk;
        break;
    case VoxelFace::LEFT:   // -X (West)
        m_NeighbourWest = chunk;
        break;
    default:
        break;
    }

    // Mark this chunk as dirty since neighbour affects face culling
    m_IsDirty = true;
}

Chunk* Chunk::GetNeighbour(VoxelFace direction) const
{
    switch (direction)
    {
    case VoxelFace::FRONT:  return m_NeighbourNorth;
    case VoxelFace::BACK:   return m_NeighbourSouth;
    case VoxelFace::RIGHT:  return m_NeighbourEast;
    case VoxelFace::LEFT:   return m_NeighbourWest;
    default:                return nullptr;
    }
}

bool Chunk::HasNeighbour(VoxelFace direction) const
{
    return GetNeighbour(direction) != nullptr;
}

// Update empty status by checking if all voxels are air
void Chunk::UpdateEmptyStatus()
{
    m_IsEmpty = true;

    for (const auto& voxel : m_Voxels)
    {
        if (!voxel.IsAir())
        {
            m_IsEmpty = false;
            return;
        }
    }
}

// Set mesh data
void Chunk::SetMeshData(std::unique_ptr<VertexArray> vao,
    std::unique_ptr<VertexBuffer> vbo,
    std::unique_ptr<IndexBuffer> ibo,
    int vertexCount,
    int indexCount)
{
    m_VAO = std::move(vao);
    m_VBO = std::move(vbo);
    m_IBO = std::move(ibo);
    m_VertexCount = vertexCount;
    m_IndexCount = indexCount;
    m_HasMesh = true;
    m_IsDirty = false;
}

// Clear mesh data
void Chunk::ClearMesh()
{
    m_VAO.reset();
    m_VBO.reset();
    m_IBO.reset();
    m_VertexCount = 0;
    m_IndexCount = 0;
    m_HasMesh = false;
}

// Fill entire chunk with a single voxel type
void Chunk::Fill(VoxelType type)
{
    Voxel voxel(type);
    for (auto& v : m_Voxels)
    {
        v = voxel;
    }

    m_IsEmpty = (type == VoxelType::AIR);
    m_IsDirty = true;
}

// Fill a single layer
void Chunk::FillLayer(int y, VoxelType type)
{
    if (y < 0 || y >= CHUNK_HEIGHT)
        return;

    Voxel voxel(type);

    for (int x = 0; x < CHUNK_SIZE; ++x)
    {
        for (int z = 0; z < CHUNK_SIZE; ++z)
        {
            SetVoxel(x, y, z, voxel);
        }
    }

    m_IsDirty = true;
}

// Fill a range of layers
void Chunk::FillLayers(int yStart, int yEnd, VoxelType type)
{
    for (int y = yStart; y <= yEnd; ++y)
    {
        FillLayer(y, type);
    }
}

// Clear all voxels to air
void Chunk::Clear()
{
    Fill(VoxelType::AIR);
    m_IsEmpty = true;
    m_IsDirty = true;
}

// Check if a voxel face is exposed (needs rendering)
bool Chunk::IsFaceVisible(int x, int y, int z, VoxelFace face) const
{
    const Voxel& currentVoxel = GetVoxel(x, y, z);

    // Air voxels don't have visible faces
    if (currentVoxel.IsAir())
        return false;

    // Calculate neighbour position
    int nx = x;
    int ny = y;
    int nz = z;

    switch (face)
    {
    case VoxelFace::FRONT:  nz++; break;  // +Z
    case VoxelFace::BACK:   nz--; break;  // -Z
    case VoxelFace::RIGHT:  nx++; break;  // +X
    case VoxelFace::LEFT:   nx--; break;  // -X
    case VoxelFace::TOP:    ny++; break;  // +Y
    case VoxelFace::BOTTOM: ny--; break;  // -Y
    default: return false;
    }

    // Check if neighbour is within this chunk
    if (IsValidPosition(nx, ny, nz))
    {
        const Voxel& neighbourVoxel = GetVoxel(nx, ny, nz);

        // Face is visible if neighbour is air or transparent
        return neighbourVoxel.IsAir() ||
            (neighbourVoxel.IsTransparent() && !currentVoxel.IsTransparent());
    }

    // Check neighbouring chunks for boundary faces
    if (ny < 0)
    {
        // Bottom of world - always render
        return true;
    }

    if (ny >= CHUNK_HEIGHT)
    {
        // Top of world - always render
        return true;
    }

    // Check horizontal neighbours
    Chunk* neighbourChunk = nullptr;
    int neighbourX = x;
    int neighbourZ = z;

    if (nx < 0)
    {
        // West neighbour
        neighbourChunk = m_NeighbourWest;
        neighbourX = CHUNK_SIZE - 1;
    }
    else if (nx >= CHUNK_SIZE)
    {
        // East neighbour
        neighbourChunk = m_NeighbourEast;
        neighbourX = 0;
    }
    else if (nz < 0)
    {
        // South neighbour
        neighbourChunk = m_NeighbourSouth;
        neighbourZ = CHUNK_SIZE - 1;
    }
    else if (nz >= CHUNK_SIZE)
    {
        // North neighbour
        neighbourChunk = m_NeighbourNorth;
        neighbourZ = 0;
    }

    if (neighbourChunk != nullptr)
    {
        const Voxel& neighbourVoxel = neighbourChunk->GetVoxel(neighbourX, ny, neighbourZ);
        return neighbourVoxel.IsAir() ||
            (neighbourVoxel.IsTransparent() && !currentVoxel.IsTransparent());
    }

    // No neighbour chunk loaded - assume face is visible
    return true;
}