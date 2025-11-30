#pragma once

#include <cstdint>
#include <string>

// Enumeration of all voxel types in the world
enum class VoxelType : uint8_t
{
    AIR = 0,
    STONE,
    DIRT,
    GRASS,
    SAND,
    WATER,
    WOOD,
    LEAVES,
    GLASS,
    COBBLESTONE,
    PLANKS,
    BEDROCK,
    GRAVEL,
    BRICK,
    SNOW,
    ICE,
    COUNT // Used to track total number of voxel types
};

// Face directions for voxels
enum class VoxelFace : uint8_t
{
    FRONT = 0,  // +Z
    BACK,       // -Z
    LEFT,       // -X
    RIGHT,      // +X
    TOP,        // +Y
    BOTTOM,     // -Y
    COUNT
};

// Properties that define voxel behaviour and appearance
struct VoxelProperties
{
    bool isSolid;           // Can player collide with it
    bool isOpaque;          // Does it block light completely
    bool isTransparent;     // Can you see through it (glass, water)
    bool isLiquid;          // Does it flow
    bool isEmissive;        // Does it emit light
    uint8_t lightLevel;     // Light emission level (0-15)
    float breakTime;        // Time to break in seconds

    // Texture indices in the texture atlas for each face
    // If all faces are the same, use textureTop for all
    uint16_t textureTop;
    uint16_t textureBottom;
    uint16_t textureSides;

    VoxelProperties()
        : isSolid(true)
        , isOpaque(true)
        , isTransparent(false)
        , isLiquid(false)
        , isEmissive(false)
        , lightLevel(0)
        , breakTime(1.0f)
        , textureTop(0)
        , textureBottom(0)
        , textureSides(0)
    {
    }
};

// Main voxel class
class Voxel
{
private:
    VoxelType m_Type;
    uint8_t m_Metadata; // For additional state (growth stage, rotation, etc.)

    // Static array of properties for each voxel type
    static VoxelProperties s_Properties[static_cast<size_t>(VoxelType::COUNT)];
    static bool s_Initialised;

    // Initialise all voxel properties (called once)
    static void InitialiseProperties();

public:
    // Constructors
    Voxel();
    explicit Voxel(VoxelType type);
    Voxel(VoxelType type, uint8_t metadata);

    // Getters
    VoxelType GetType() const { return m_Type; }
    uint8_t GetMetadata() const { return m_Metadata; }

    // Setters
    void SetType(VoxelType type) { m_Type = type; }
    void SetMetadata(uint8_t metadata) { m_Metadata = metadata; }

    // Property queries
    bool IsSolid() const;
    bool IsOpaque() const;
    bool IsTransparent() const;
    bool IsLiquid() const;
    bool IsEmissive() const;
    bool IsAir() const { return m_Type == VoxelType::AIR; }

    uint8_t GetLightLevel() const;
    float GetBreakTime() const;

    // Texture queries
    uint16_t GetTextureIndex(VoxelFace face) const;

    // Static property access
    static const VoxelProperties& GetProperties(VoxelType type);

    // Utility
    static std::string GetVoxelName(VoxelType type);

    // Ensure properties are initialised
    static void EnsureInitialised();
};