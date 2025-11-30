#include "Voxel.h"

// Static member initialisation
VoxelProperties Voxel::s_Properties[static_cast<size_t>(VoxelType::COUNT)];
bool Voxel::s_Initialised = false;

// Default constructor - creates air voxel
Voxel::Voxel()
    : m_Type(VoxelType::AIR)
    , m_Metadata(0)
{
    EnsureInitialised();
}

// Constructor with type
Voxel::Voxel(VoxelType type)
    : m_Type(type)
    , m_Metadata(0)
{
    EnsureInitialised();
}

// Constructor with type and metadata
Voxel::Voxel(VoxelType type, uint8_t metadata)
    : m_Type(type)
    , m_Metadata(metadata)
{
    EnsureInitialised();
}

// Ensure properties are initialised before use
void Voxel::EnsureInitialised()
{
    if (!s_Initialised)
    {
        InitialiseProperties();
        s_Initialised = true;
    }
}

// Initialise all voxel type properties
void Voxel::InitialiseProperties()
{
    // AIR - completely empty, non-solid, transparent
    s_Properties[static_cast<size_t>(VoxelType::AIR)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::AIR)].isSolid = false;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].isOpaque = false;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].isTransparent = true;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].breakTime = 0.0f;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].textureTop = 0;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].textureBottom = 0;
    s_Properties[static_cast<size_t>(VoxelType::AIR)].textureSides = 0;

    // STONE - basic solid block
    s_Properties[static_cast<size_t>(VoxelType::STONE)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::STONE)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::STONE)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::STONE)].breakTime = 1.5f;
    s_Properties[static_cast<size_t>(VoxelType::STONE)].textureTop = 1;
    s_Properties[static_cast<size_t>(VoxelType::STONE)].textureBottom = 1;
    s_Properties[static_cast<size_t>(VoxelType::STONE)].textureSides = 1;

    // DIRT - has same texture on all sides
    s_Properties[static_cast<size_t>(VoxelType::DIRT)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].breakTime = 0.5f;
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].textureTop = 2;
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].textureBottom = 2;
    s_Properties[static_cast<size_t>(VoxelType::DIRT)].textureSides = 2;

    // GRASS - different textures on top, bottom, and sides
    s_Properties[static_cast<size_t>(VoxelType::GRASS)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].breakTime = 0.6f;
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].textureTop = 3;      // Grass top
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].textureBottom = 2;   // Dirt bottom
    s_Properties[static_cast<size_t>(VoxelType::GRASS)].textureSides = 4;    // Grass side

    // SAND - desert/beach block
    s_Properties[static_cast<size_t>(VoxelType::SAND)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::SAND)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::SAND)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::SAND)].breakTime = 0.5f;
    s_Properties[static_cast<size_t>(VoxelType::SAND)].textureTop = 5;
    s_Properties[static_cast<size_t>(VoxelType::SAND)].textureBottom = 5;
    s_Properties[static_cast<size_t>(VoxelType::SAND)].textureSides = 5;

    // WATER - transparent, liquid
    s_Properties[static_cast<size_t>(VoxelType::WATER)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::WATER)].isSolid = false;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].isOpaque = false;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].isTransparent = true;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].isLiquid = true;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].breakTime = 0.0f;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].textureTop = 6;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].textureBottom = 6;
    s_Properties[static_cast<size_t>(VoxelType::WATER)].textureSides = 6;

    // WOOD - log texture
    s_Properties[static_cast<size_t>(VoxelType::WOOD)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].breakTime = 2.0f;
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].textureTop = 7;      // Log top
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].textureBottom = 7;   // Log top
    s_Properties[static_cast<size_t>(VoxelType::WOOD)].textureSides = 8;    // Log side

    // LEAVES - semi-transparent foliage
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].isOpaque = false;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].isTransparent = true;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].breakTime = 0.2f;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].textureTop = 9;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].textureBottom = 9;
    s_Properties[static_cast<size_t>(VoxelType::LEAVES)].textureSides = 9;

    // GLASS - fully transparent
    s_Properties[static_cast<size_t>(VoxelType::GLASS)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].isOpaque = false;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].isTransparent = true;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].breakTime = 0.3f;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].textureTop = 10;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].textureBottom = 10;
    s_Properties[static_cast<size_t>(VoxelType::GLASS)].textureSides = 10;

    // COBBLESTONE
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].breakTime = 2.0f;
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].textureTop = 11;
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].textureBottom = 11;
    s_Properties[static_cast<size_t>(VoxelType::COBBLESTONE)].textureSides = 11;

    // PLANKS - processed wood
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].breakTime = 2.0f;
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].textureTop = 12;
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].textureBottom = 12;
    s_Properties[static_cast<size_t>(VoxelType::PLANKS)].textureSides = 12;

    // BEDROCK - indestructible
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].breakTime = -1.0f; // Unbreakable
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].textureTop = 13;
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].textureBottom = 13;
    s_Properties[static_cast<size_t>(VoxelType::BEDROCK)].textureSides = 13;

    // GRAVEL
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].breakTime = 0.6f;
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].textureTop = 14;
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].textureBottom = 14;
    s_Properties[static_cast<size_t>(VoxelType::GRAVEL)].textureSides = 14;

    // BRICK
    s_Properties[static_cast<size_t>(VoxelType::BRICK)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].breakTime = 2.0f;
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].textureTop = 15;
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].textureBottom = 15;
    s_Properties[static_cast<size_t>(VoxelType::BRICK)].textureSides = 15;

    // SNOW
    s_Properties[static_cast<size_t>(VoxelType::SNOW)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].isOpaque = true;
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].breakTime = 0.2f;
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].textureTop = 16;
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].textureBottom = 16;
    s_Properties[static_cast<size_t>(VoxelType::SNOW)].textureSides = 16;

    // ICE - transparent and slippery
    s_Properties[static_cast<size_t>(VoxelType::ICE)] = VoxelProperties();
    s_Properties[static_cast<size_t>(VoxelType::ICE)].isSolid = true;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].isOpaque = false;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].isTransparent = true;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].breakTime = 0.5f;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].textureTop = 17;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].textureBottom = 17;
    s_Properties[static_cast<size_t>(VoxelType::ICE)].textureSides = 17;
}

// Property query methods
bool Voxel::IsSolid() const
{
    return s_Properties[static_cast<size_t>(m_Type)].isSolid;
}

bool Voxel::IsOpaque() const
{
    return s_Properties[static_cast<size_t>(m_Type)].isOpaque;
}

bool Voxel::IsTransparent() const
{
    return s_Properties[static_cast<size_t>(m_Type)].isTransparent;
}

bool Voxel::IsLiquid() const
{
    return s_Properties[static_cast<size_t>(m_Type)].isLiquid;
}

bool Voxel::IsEmissive() const
{
    return s_Properties[static_cast<size_t>(m_Type)].isEmissive;
}

uint8_t Voxel::GetLightLevel() const
{
    return s_Properties[static_cast<size_t>(m_Type)].lightLevel;
}

float Voxel::GetBreakTime() const
{
    return s_Properties[static_cast<size_t>(m_Type)].breakTime;
}

uint16_t Voxel::GetTextureIndex(VoxelFace face) const
{
    const VoxelProperties& props = s_Properties[static_cast<size_t>(m_Type)];

    switch (face)
    {
    case VoxelFace::TOP:
        return props.textureTop;
    case VoxelFace::BOTTOM:
        return props.textureBottom;
    case VoxelFace::FRONT:
    case VoxelFace::BACK:
    case VoxelFace::LEFT:
    case VoxelFace::RIGHT:
        return props.textureSides;
    default:
        return props.textureSides;
    }
}

const VoxelProperties& Voxel::GetProperties(VoxelType type)
{
    EnsureInitialised();
    return s_Properties[static_cast<size_t>(type)];
}

std::string Voxel::GetVoxelName(VoxelType type)
{
    switch (type)
    {
    case VoxelType::AIR:          return "Air";
    case VoxelType::STONE:        return "Stone";
    case VoxelType::DIRT:         return "Dirt";
    case VoxelType::GRASS:        return "Grass";
    case VoxelType::SAND:         return "Sand";
    case VoxelType::WATER:        return "Water";
    case VoxelType::WOOD:         return "Wood";
    case VoxelType::LEAVES:       return "Leaves";
    case VoxelType::GLASS:        return "Glass";
    case VoxelType::COBBLESTONE:  return "Cobblestone";
    case VoxelType::PLANKS:       return "Planks";
    case VoxelType::BEDROCK:      return "Bedrock";
    case VoxelType::GRAVEL:       return "Gravel";
    case VoxelType::BRICK:        return "Brick";
    case VoxelType::SNOW:         return "Snow";
    case VoxelType::ICE:          return "Ice";
    default:                      return "Unknown";
    }
}