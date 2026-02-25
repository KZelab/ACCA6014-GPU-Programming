#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "ModelMesh.h"
#include "../Shader.h"

#include <assimp/scene.h>
#include <glm/glm.hpp>

// ----------------------------------------------------------------------------
// Model
// ----------------------------------------------------------------------------
// Loads a 3D model file via Assimp and stores the result as a collection of
// ModelMesh objects.
//
// Assimp organises a scene as a tree of aiNode objects.  Each node references
// one or more aiMesh objects held in the aiScene.  This class walks that tree,
// converts each aiMesh into a ModelMesh, and creates Texture objects for any
// referenced image files using the existing Texture class.
//
// Texture deduplication:
//   m_TexturesLoaded is a cache keyed on the absolute image path.  When
//   multiple meshes reference the same image, the same shared_ptr<Texture>
//   is reused and the image is uploaded to the GPU only once.
//
// UV flip note:
//   Texture::Texture() always calls stbi_set_flip_vertically_on_load(1), so
//   the V coordinate is already corrected at image-decode time.  The Assimp
//   aiProcess_FlipUVs flag must NOT also be used or the flip will cancel out.
//   The flipUVs constructor parameter therefore defaults to false.
//
// Usage:
//   Model backpack("assets/backpack/backpack.obj");
//
//   // Inside the render loop:
//   shader.Bind();
//   shader.setMat4("u_Model", backpack.getTransformMatrix());
//   backpack.Draw(shader);
// ----------------------------------------------------------------------------

class Model
{
public:
    // -------------------------------------------------------------------------
    // Constructor
    // -------------------------------------------------------------------------
    // path    - path to the model file (OBJ, FBX, GLTF, etc.)
    // flipUVs - applies aiProcess_FlipUVs during import.  Leave as false
    //           (the default) because Texture already flips via stb_image.
    //           Only set to true if you are loading textures through a path
    //           that does NOT flip, and you are certain there is no double-flip.
    // -------------------------------------------------------------------------
    explicit Model(const std::string& path, bool flipUVs = false);

    // -------------------------------------------------------------------------
    // Draw
    // -------------------------------------------------------------------------
    // Calls ModelMesh::Draw(shader) for every mesh in the model.
    // Set any per-model shader uniforms (e.g. the model matrix) before calling.
    // -------------------------------------------------------------------------
    void Draw(Shader& shader);

    // -------------------------------------------------------------------------
    // Transform helpers
    // -------------------------------------------------------------------------
    // Propagates the transform to every child mesh so the whole model moves
    // as a single unit.  For independent per-mesh transforms, use getMeshes().
    // -------------------------------------------------------------------------
    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);
    void setScale(const glm::vec3& scale);

    glm::vec3 getPosition() const { return m_Position; }
    glm::vec3 getRotation() const { return m_Rotation; }
    glm::vec3 getScale()    const { return m_Scale; }

    glm::mat4 getTransformMatrix() const;

    // -------------------------------------------------------------------------
    // Accessors
    // -------------------------------------------------------------------------
    const std::vector<ModelMesh>& getMeshes() const { return m_Meshes; }
    std::size_t getMeshCount()                const { return m_Meshes.size(); }

private:
    std::vector<ModelMesh> m_Meshes;
    std::string            m_Directory;

    glm::vec3 m_Position{ 0.0f };
    glm::vec3 m_Rotation{ 0.0f };
    glm::vec3 m_Scale{ 1.0f };

    // Cache maps absolute image path -> shared Texture object.
    // shared_ptr allows the cache and each ModelMesh to share ownership;
    // the GPU resource is released when the last reference is dropped.
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_TexturesLoaded;

    // -------------------------------------------------------------------------
    // Private loading helpers
    // -------------------------------------------------------------------------
    void loadModel(const std::string& path, bool flipUVs);
    void processNode(const aiNode* node, const aiScene* scene);

    ModelMesh processMesh(const aiMesh* mesh, const aiScene* scene);

    std::vector<MeshTexture> loadMaterialTextures(const aiMaterial* material,
        aiTextureType      type,
        const std::string& typeName);
};