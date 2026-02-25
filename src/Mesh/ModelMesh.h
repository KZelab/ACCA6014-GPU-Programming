#pragma once
#include "Mesh.h"
#include "../Shader.h"
#include "../Texture.h"
#include <vector>
#include <string>
#include <memory>

// ----------------------------------------------------------------------------
// MeshTexture
// ----------------------------------------------------------------------------
// Wraps a shared_ptr to the existing Texture class so that GPU resources are
// managed through the same RAII path used everywhere else in the project.
//
// shared_ptr is used rather than unique_ptr because the Model texture cache
// and the individual ModelMesh objects both need to refer to the same
// Texture object without re-uploading it.  The texture is destroyed
// automatically when the last ModelMesh and the cache both release it.
//
// 'type' is the GLSL sampler name prefix used to build uniform names:
//   "texture_diffuse"   -> texture_diffuse1, texture_diffuse2, ...
//   "texture_specular"  -> texture_specular1, ...
//   "texture_normal"    -> texture_normal1, ...
//
// 'path' is the resolved file path used as a deduplication key in Model.
// ----------------------------------------------------------------------------
struct MeshTexture
{
    std::shared_ptr<Texture> texture;
    std::string              type;
    std::string              path;
};

// ----------------------------------------------------------------------------
// ModelMesh
// ----------------------------------------------------------------------------
// Extends Mesh with a vector of textures and a shader-aware Draw overload.
// Mesh itself is not modified; all GPU buffer setup is delegated to it via
// the base-class constructor.
//
// Draw(Shader&) calls texture->Bind(slot) for each texture (which activates
// the correct texture unit internally), sets the matching sampler uniform on
// the shader, then issues the draw call via the base Renderer.
// ----------------------------------------------------------------------------
class ModelMesh : public Mesh
{
public:
    ModelMesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const std::vector<MeshTexture>& textures);

    // Rule of Five.
    // Move operations are explicitly defaulted because the base class virtual
    // destructor suppresses their implicit generation.
    // Copy operations are deleted because Mesh contains unique_ptr members.
    ModelMesh(ModelMesh&&) = default;
    ModelMesh& operator=(ModelMesh&&) = default;

    ModelMesh(const ModelMesh&) = delete;
    ModelMesh& operator=(const ModelMesh&) = delete;

    // Shader-aware draw: binds textures, sets sampler uniforms, draws.
    // Declare matching uniforms in your GLSL fragment shader:
    //   uniform sampler2D texture_diffuse1;
    //   uniform sampler2D texture_specular1;
    //   uniform sampler2D texture_normal1;
    void Draw(Shader& shader);

private:
    std::vector<MeshTexture> m_Textures;
};