#include "ChunkMeshBuilder.h"
#include "../VertexArray.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../VertexBufferLayout.h"
#include <cmath>
#include <iostream>

ChunkMeshBuilder::ChunkMeshBuilder(Chunk* chunk)
    : m_Chunk(chunk)
    , m_CurrentVertexCount(0)
{
    // Reserve space to reduce allocations
    m_Vertices.reserve(4096);  // Typical chunk might have ~1000-2000 vertices
    m_Indices.reserve(8192);   // Typical chunk might have ~2000-4000 triangles
}

ChunkMeshBuilder::~ChunkMeshBuilder()
{
}

void ChunkMeshBuilder::Clear()
{
    m_Vertices.clear();
    m_Indices.clear();
    m_CurrentVertexCount = 0;
}

void ChunkMeshBuilder::BuildMesh()
{
    Clear();

    // Early exit if chunk is empty
    if (m_Chunk->IsEmpty())
    {
        return;
    }

    // Iterate through all voxels in the chunk
    for (int y = 0; y < CHUNK_HEIGHT; ++y)
    {
        for (int z = 0; z < CHUNK_SIZE; ++z)
        {
            for (int x = 0; x < CHUNK_SIZE; ++x)
            {
                const Voxel& voxel = m_Chunk->GetVoxel(x, y, z);

                // Skip air voxels
                if (voxel.IsAir())
                    continue;

                // Check each face and add it if visible
                for (int faceIdx = 0; faceIdx < static_cast<int>(VoxelFace::COUNT); ++faceIdx)
                {
                    VoxelFace face = static_cast<VoxelFace>(faceIdx);

                    if (m_Chunk->IsFaceVisible(x, y, z, face))
                    {
                        uint16_t textureIndex = voxel.GetTextureIndex(face);
                        AddFace(x, y, z, face, textureIndex);
                    }
                }
            }
        }
    }
}

void ChunkMeshBuilder::BuildMeshGreedy()
{
    // For now, just use the simple method
    // Greedy meshing is an optimization we can add later
    BuildMesh();
}

void ChunkMeshBuilder::ApplyMeshToChunk()
{
    // If no vertices, clear the mesh and return
    if (m_Vertices.empty())
    {
        m_Chunk->ClearMesh();
        return;
    }

    // Create vertex buffer
    auto vbo = std::make_unique<VertexBuffer>(
        m_Vertices.data(),
        static_cast<unsigned int>(m_Vertices.size() * sizeof(VoxelVertex))
    );

    // Create vertex buffer layout
    VertexBufferLayout layout;
    layout.Push<float>(3);  // Position (x, y, z)
    layout.Push<float>(2);  // Texture coordinates (u, v)
    layout.Push<float>(3);  // Normal (nx, ny, nz)
    layout.Push<float>(1);  // Ambient occlusion

    // Create vertex array and add buffer
    auto vao = std::make_unique<VertexArray>();
    vao->AddBuffer(*vbo, layout);

    // Create index buffer
    auto ibo = std::make_unique<IndexBuffer>(
        m_Indices.data(),
        static_cast<unsigned int>(m_Indices.size())
    );

    // Apply to chunk
    m_Chunk->SetMeshData(
        std::move(vao),
        std::move(vbo),
        std::move(ibo),
        static_cast<int>(m_Vertices.size()),
        static_cast<int>(m_Indices.size())
    );
}

void ChunkMeshBuilder::AddFace(int x, int y, int z, VoxelFace face, uint16_t textureIndex)
{
    // Get the 4 corners of this face in world space
    float vertices[12]; // 4 vertices * 3 components
    GetFaceVertices(x, y, z, face, vertices);

    // Get texture coordinates
    float uvCoords[8]; // 4 vertices * 2 components
    GetTextureCoords(textureIndex, uvCoords);

    // Get face normal
    float normal[3];
    GetFaceNormal(face, normal);

    // Calculate ambient occlusion for each corner
    float ao[4];
    for (int i = 0; i < 4; ++i)
    {
        ao[i] = CalculateAO(x, y, z, face, i);
    }

    // Create 4 vertices for the quad
    VoxelVertex v0(
        vertices[0], vertices[1], vertices[2],     // Position
        uvCoords[0], uvCoords[1],                  // UV
        normal[0], normal[1], normal[2],           // Normal
        ao[0]                                       // AO
    );

    VoxelVertex v1(
        vertices[3], vertices[4], vertices[5],
        uvCoords[2], uvCoords[3],
        normal[0], normal[1], normal[2],
        ao[1]
    );

    VoxelVertex v2(
        vertices[6], vertices[7], vertices[8],
        uvCoords[4], uvCoords[5],
        normal[0], normal[1], normal[2],
        ao[2]
    );

    VoxelVertex v3(
        vertices[9], vertices[10], vertices[11],
        uvCoords[6], uvCoords[7],
        normal[0], normal[1], normal[2],
        ao[3]
    );

    // Add the quad
    AddQuad(v0, v1, v2, v3);
}

void ChunkMeshBuilder::AddQuad(const VoxelVertex& v0, const VoxelVertex& v1,
    const VoxelVertex& v2, const VoxelVertex& v3)
{
    unsigned int baseIndex = m_CurrentVertexCount;

    // Add vertices
    m_Vertices.push_back(v0);
    m_Vertices.push_back(v1);
    m_Vertices.push_back(v2);
    m_Vertices.push_back(v3);

    // Add indices for two triangles
    // First triangle (0, 1, 2)
    m_Indices.push_back(baseIndex + 0);
    m_Indices.push_back(baseIndex + 1);
    m_Indices.push_back(baseIndex + 2);

    // Second triangle (0, 2, 3)
    m_Indices.push_back(baseIndex + 0);
    m_Indices.push_back(baseIndex + 2);
    m_Indices.push_back(baseIndex + 3);

    m_CurrentVertexCount += 4;
}

void ChunkMeshBuilder::GetTextureCoords(uint16_t textureIndex, float uvCoords[8])
{
    // Calculate which row and column in the texture atlas
    int row = textureIndex / TEXTURES_PER_ROW;
    int col = textureIndex % TEXTURES_PER_ROW;

    // Calculate UV coordinates (0.0 to 1.0)
    float texelSize = 1.0f / static_cast<float>(TEXTURES_PER_ROW);

    float u0 = col * texelSize;
    float v0 = row * texelSize;
    float u1 = (col + 1) * texelSize;
    float v1 = (row + 1) * texelSize;

    // Add small padding to prevent texture bleeding
    float padding = 0.001f;
    u0 += padding;
    v0 += padding;
    u1 -= padding;
    v1 -= padding;

    // Debug: Print UV coordinates for first few textures
    static int debugCount = 0;
    if (debugCount < 3)
    {
        std::cout << "DEBUG: Texture index " << textureIndex
                  << " -> row=" << row << ", col=" << col
                  << " -> UV: (" << u0 << "," << v0 << ") to (" << u1 << "," << v1 << ")" << std::endl;
        debugCount++;
    }

    // Flip V coordinates (since we disabled stbi vertical flip)
    // Bottom-left, bottom-right, top-right, top-left
    uvCoords[0] = u0; uvCoords[1] = v1; // Bottom-left (flipped V)
    uvCoords[2] = u1; uvCoords[3] = v1; // Bottom-right (flipped V)
    uvCoords[4] = u1; uvCoords[5] = v0; // Top-right (flipped V)
    uvCoords[6] = u0; uvCoords[7] = v0; // Top-left (flipped V)
}

float ChunkMeshBuilder::CalculateAO(int x, int y, int z, VoxelFace face, int corner)
{
    // Simplified ambient occlusion
    // Check 3 adjacent blocks for each corner
    // Returns a value between 0.3 (fully occluded) and 1.0 (not occluded)

    int dx1 = 0, dy1 = 0, dz1 = 0;
    int dx2 = 0, dy2 = 0, dz2 = 0;
    int dx3 = 0, dy3 = 0, dz3 = 0;

    // Determine which blocks to check based on face and corner
    switch (face)
    {
    case VoxelFace::TOP: // +Y
        dy1 = 1; dy2 = 1; dy3 = 1;
        if (corner == 0) { dx1 = -1; dz2 = -1; dx3 = -1; dz3 = -1; } // Bottom-left
        if (corner == 1) { dx1 = 1;  dz2 = -1; dx3 = 1;  dz3 = -1; } // Bottom-right
        if (corner == 2) { dx1 = 1;  dz2 = 1;  dx3 = 1;  dz3 = 1; } // Top-right
        if (corner == 3) { dx1 = -1; dz2 = 1;  dx3 = -1; dz3 = 1; } // Top-left
        break;

    case VoxelFace::BOTTOM: // -Y
        dy1 = -1; dy2 = -1; dy3 = -1;
        if (corner == 0) { dx1 = -1; dz2 = -1; dx3 = -1; dz3 = -1; }
        if (corner == 1) { dx1 = 1;  dz2 = -1; dx3 = 1;  dz3 = -1; }
        if (corner == 2) { dx1 = 1;  dz2 = 1;  dx3 = 1;  dz3 = 1; }
        if (corner == 3) { dx1 = -1; dz2 = 1;  dx3 = -1; dz3 = 1; }
        break;

    case VoxelFace::FRONT: // +Z
        dz1 = 1; dz2 = 1; dz3 = 1;
        if (corner == 0) { dx1 = -1; dy2 = -1; dx3 = -1; dy3 = -1; }
        if (corner == 1) { dx1 = 1;  dy2 = -1; dx3 = 1;  dy3 = -1; }
        if (corner == 2) { dx1 = 1;  dy2 = 1;  dx3 = 1;  dy3 = 1; }
        if (corner == 3) { dx1 = -1; dy2 = 1;  dx3 = -1; dy3 = 1; }
        break;

    case VoxelFace::BACK: // -Z
        dz1 = -1; dz2 = -1; dz3 = -1;
        if (corner == 0) { dx1 = 1;  dy2 = -1; dx3 = 1;  dy3 = -1; }
        if (corner == 1) { dx1 = -1; dy2 = -1; dx3 = -1; dy3 = -1; }
        if (corner == 2) { dx1 = -1; dy2 = 1;  dx3 = -1; dy3 = 1; }
        if (corner == 3) { dx1 = 1;  dy2 = 1;  dx3 = 1;  dy3 = 1; }
        break;

    case VoxelFace::RIGHT: // +X
        dx1 = 1; dx2 = 1; dx3 = 1;
        if (corner == 0) { dz1 = -1; dy2 = -1; dz3 = -1; dy3 = -1; }
        if (corner == 1) { dz1 = 1;  dy2 = -1; dz3 = 1;  dy3 = -1; }
        if (corner == 2) { dz1 = 1;  dy2 = 1;  dz3 = 1;  dy3 = 1; }
        if (corner == 3) { dz1 = -1; dy2 = 1;  dz3 = -1; dy3 = 1; }
        break;

    case VoxelFace::LEFT: // -X
        dx1 = -1; dx2 = -1; dx3 = -1;
        if (corner == 0) { dz1 = 1;  dy2 = -1; dz3 = 1;  dy3 = -1; }
        if (corner == 1) { dz1 = -1; dy2 = -1; dz3 = -1; dy3 = -1; }
        if (corner == 2) { dz1 = -1; dy2 = 1;  dz3 = -1; dy3 = 1; }
        if (corner == 3) { dz1 = 1;  dy2 = 1;  dz3 = 1;  dy3 = 1; }
        break;

    default:
        return 1.0f;
    }

    // Check if blocks are solid
    int occluded = 0;
    if (IsOccluding(x + dx1, y + dy1, z + dz1)) occluded++;
    if (IsOccluding(x + dx2, y + dy2, z + dz2)) occluded++;
    if (IsOccluding(x + dx3, y + dy3, z + dz3)) occluded++;

    // Calculate AO value
    // 0 occluding blocks = 1.0 (full bright)
    // 1 occluding block = 0.8
    // 2 occluding blocks = 0.6
    // 3 occluding blocks = 0.4 (darkest)
    float aoValue = 1.0f - (occluded * 0.2f);
    return aoValue;
}

bool ChunkMeshBuilder::IsOccluding(int x, int y, int z) const
{
    // Check if position is within chunk bounds
    if (Chunk::IsValidPosition(x, y, z))
    {
        const Voxel& voxel = m_Chunk->GetVoxel(x, y, z);
        return voxel.IsSolid() && voxel.IsOpaque();
    }

    // Could check neighbouring chunks here for more accurate AO
    // For now, assume out-of-bounds blocks don't occlude
    return false;
}

void ChunkMeshBuilder::GetFaceNormal(VoxelFace face, float normal[3])
{
    switch (face)
    {
    case VoxelFace::FRONT:  // +Z
        normal[0] = 0.0f; normal[1] = 0.0f; normal[2] = 1.0f;
        break;
    case VoxelFace::BACK:   // -Z
        normal[0] = 0.0f; normal[1] = 0.0f; normal[2] = -1.0f;
        break;
    case VoxelFace::RIGHT:  // +X
        normal[0] = 1.0f; normal[1] = 0.0f; normal[2] = 0.0f;
        break;
    case VoxelFace::LEFT:   // -X
        normal[0] = -1.0f; normal[1] = 0.0f; normal[2] = 0.0f;
        break;
    case VoxelFace::TOP:    // +Y
        normal[0] = 0.0f; normal[1] = 1.0f; normal[2] = 0.0f;
        break;
    case VoxelFace::BOTTOM: // -Y
        normal[0] = 0.0f; normal[1] = -1.0f; normal[2] = 0.0f;
        break;
    default:
        normal[0] = 0.0f; normal[1] = 0.0f; normal[2] = 0.0f;
        break;
    }
}

void ChunkMeshBuilder::GetFaceVertices(int x, int y, int z, VoxelFace face, float vertices[12])
{
    // Convert local coordinates to world coordinates
    float worldX = static_cast<float>(m_Chunk->GetWorldX() + x);
    float worldY = static_cast<float>(y);
    float worldZ = static_cast<float>(m_Chunk->GetWorldZ() + z);

    // Each voxel is 1 unit in size
    switch (face)
    {
    case VoxelFace::FRONT: // +Z face
        // Bottom-left, bottom-right, top-right, top-left
        vertices[0] = worldX;       vertices[1] = worldY;       vertices[2] = worldZ + 1.0f;
        vertices[3] = worldX + 1.0f; vertices[4] = worldY;       vertices[5] = worldZ + 1.0f;
        vertices[6] = worldX + 1.0f; vertices[7] = worldY + 1.0f; vertices[8] = worldZ + 1.0f;
        vertices[9] = worldX;       vertices[10] = worldY + 1.0f; vertices[11] = worldZ + 1.0f;
        break;

    case VoxelFace::BACK: // -Z face
        vertices[0] = worldX + 1.0f; vertices[1] = worldY;       vertices[2] = worldZ;
        vertices[3] = worldX;       vertices[4] = worldY;       vertices[5] = worldZ;
        vertices[6] = worldX;       vertices[7] = worldY + 1.0f; vertices[8] = worldZ;
        vertices[9] = worldX + 1.0f; vertices[10] = worldY + 1.0f; vertices[11] = worldZ;
        break;

    case VoxelFace::RIGHT: // +X face
        vertices[0] = worldX + 1.0f; vertices[1] = worldY;       vertices[2] = worldZ;
        vertices[3] = worldX + 1.0f; vertices[4] = worldY;       vertices[5] = worldZ + 1.0f;
        vertices[6] = worldX + 1.0f; vertices[7] = worldY + 1.0f; vertices[8] = worldZ + 1.0f;
        vertices[9] = worldX + 1.0f; vertices[10] = worldY + 1.0f; vertices[11] = worldZ;
        break;

    case VoxelFace::LEFT: // -X face
        vertices[0] = worldX;       vertices[1] = worldY;       vertices[2] = worldZ + 1.0f;
        vertices[3] = worldX;       vertices[4] = worldY;       vertices[5] = worldZ;
        vertices[6] = worldX;       vertices[7] = worldY + 1.0f; vertices[8] = worldZ;
        vertices[9] = worldX;       vertices[10] = worldY + 1.0f; vertices[11] = worldZ + 1.0f;
        break;

    case VoxelFace::TOP: // +Y face
        vertices[0] = worldX;       vertices[1] = worldY + 1.0f; vertices[2] = worldZ;
        vertices[3] = worldX + 1.0f; vertices[4] = worldY + 1.0f; vertices[5] = worldZ;
        vertices[6] = worldX + 1.0f; vertices[7] = worldY + 1.0f; vertices[8] = worldZ + 1.0f;
        vertices[9] = worldX;       vertices[10] = worldY + 1.0f; vertices[11] = worldZ + 1.0f;
        break;

    case VoxelFace::BOTTOM: // -Y face
        vertices[0] = worldX;       vertices[1] = worldY;       vertices[2] = worldZ + 1.0f;
        vertices[3] = worldX + 1.0f; vertices[4] = worldY;       vertices[5] = worldZ + 1.0f;
        vertices[6] = worldX + 1.0f; vertices[7] = worldY;       vertices[8] = worldZ;
        vertices[9] = worldX;       vertices[10] = worldY;       vertices[11] = worldZ;
        break;

    default:
        break;
    }
}