#include "TextureAtlas.h"
#include "Renderer.h"
#include <iostream>

TextureAtlas::TextureAtlas()
    : m_AtlasSize(256)
    , m_TextureSize(16)
    , m_TexturesPerRow(16)
{
}

TextureAtlas::~TextureAtlas()
{
}

bool TextureAtlas::LoadAtlas(const std::string& filepath, int textureSize)
{
    m_TextureSize = textureSize;

    try
    {
        // Create the texture from file using your existing Texture class
        m_Atlas = std::make_unique<Texture>(filepath);

        // Calculate atlas parameters based on loaded texture
        m_AtlasSize = m_Atlas->getWidth();
        m_TexturesPerRow = m_AtlasSize / m_TextureSize;

        std::cout << "Loaded texture atlas: " << filepath << std::endl;
        std::cout << "Atlas size: " << m_AtlasSize << "x" << m_AtlasSize << std::endl;
        std::cout << "Texture size: " << m_TextureSize << "x" << m_TextureSize << std::endl;
        std::cout << "Textures per row: " << m_TexturesPerRow << std::endl;

        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "Error loading texture atlas: " << e.what() << std::endl;
        return false;
    }
}

void TextureAtlas::RegisterTexture(const std::string& name, uint16_t index)
{
    m_TextureMap[name] = index;
}

uint16_t TextureAtlas::GetTextureIndex(const std::string& name) const
{
    auto it = m_TextureMap.find(name);
    if (it != m_TextureMap.end())
    {
        return it->second;
    }

    std::cout << "Warning: Texture '" << name << "' not found in atlas, using index 0" << std::endl;
    return 0; // Default to first texture (air)
}

void TextureAtlas::Bind(unsigned int slot) const
{
    if (m_Atlas)
    {
        m_Atlas->Bind(slot);
    }
}

void TextureAtlas::Unbind() const
{
    if (m_Atlas)
    {
        m_Atlas->Unbind();
    }
}

void TextureAtlas::GetUVCoords(uint16_t index, float& u0, float& v0, float& u1, float& v1) const
{
    // Calculate which row and column in the texture atlas
    int row = index / m_TexturesPerRow;
    int col = index % m_TexturesPerRow;

    // Calculate UV coordinates (0.0 to 1.0)
    float texelSize = 1.0f / static_cast<float>(m_TexturesPerRow);

    u0 = col * texelSize;
    v0 = row * texelSize;
    u1 = (col + 1) * texelSize;
    v1 = (row + 1) * texelSize;

    // Add small padding to prevent texture bleeding
    float padding = 0.001f;
    u0 += padding;
    v0 += padding;
    u1 -= padding;
    v1 -= padding;
}