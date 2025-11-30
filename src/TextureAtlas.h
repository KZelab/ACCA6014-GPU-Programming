#pragma once

#include "Texture.h"
#include <string>
#include <unordered_map>
#include <memory>

// Texture atlas manager for voxel textures
// Combines multiple small textures into a single large texture for efficient rendering
class TextureAtlas
{
private:
    std::unique_ptr<Texture> m_Atlas;

    int m_AtlasSize;           // Size of atlas texture (e.g., 256x256)
    int m_TextureSize;         // Size of individual textures (e.g., 16x16)
    int m_TexturesPerRow;      // Number of textures per row/column

    // Map of texture names to their index in the atlas
    std::unordered_map<std::string, uint16_t> m_TextureMap;

public:
    TextureAtlas();
    ~TextureAtlas();

    // Load a texture atlas from file (e.g., Kenney atlas)
    bool LoadAtlas(const std::string& filepath, int textureSize = 16);

    // Get texture index by name
    uint16_t GetTextureIndex(const std::string& name) const;

    // Register a texture name with an index
    void RegisterTexture(const std::string& name, uint16_t index);

    // Bind the atlas texture for rendering
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    // Getters
    int GetAtlasSize() const { return m_AtlasSize; }
    int GetTextureSize() const { return m_TextureSize; }
    int GetTexturesPerRow() const { return m_TexturesPerRow; }

    // Calculate UV coordinates for a texture index
    void GetUVCoords(uint16_t index, float& u0, float& v0, float& u1, float& v1) const;

    // Get the underlying texture
    Texture* GetTexture() const { return m_Atlas.get(); }
};