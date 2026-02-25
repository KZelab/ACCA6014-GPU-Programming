#include "ModelMesh.h"
#include "../Renderer.h"

#include <GL/glew.h>
#include <iostream>

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
// Delegates vertex/index data to the base-class constructor so that SetupMesh
// is called and the VAO/VBO/EBO are created.  Textures are stored separately
// because the base class has no knowledge of them.
// ----------------------------------------------------------------------------

ModelMesh::ModelMesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const std::vector<MeshTexture>& textures)
    : Mesh(vertices, indices),
    m_Textures(textures)
{
}

// ----------------------------------------------------------------------------
// Draw(Shader&)
// ----------------------------------------------------------------------------
//
// Texture unit assignment:
//   Texture::Bind(slot) calls glActiveTexture(GL_TEXTURE0 + slot) and then
//   glBindTexture internally, so we do not need to manage the active texture
//   unit manually here.  We simply pass the loop index as the slot.
//
//   Counters per type produce uniform names starting at 1:
//     texture_diffuse1  -> slot 0
//     texture_diffuse2  -> slot 1
//     texture_specular1 -> slot 2
//     texture_normal1   -> slot 3
//   etc.
//
// After the draw call, each texture unit is unbound to avoid leaking state
// into subsequent rendering passes.
// ----------------------------------------------------------------------------

void ModelMesh::Draw(Shader& shader)
{
    if (!m_VAO || !m_EBO)
    {
        std::cerr << "ModelMesh::Draw(Shader&) called before SetupMesh()!\n";
        return;
    }

    unsigned int diffuseIndex = 1;
    unsigned int specularIndex = 1;
    unsigned int normalIndex = 1;

    const auto count = static_cast<unsigned int>(m_Textures.size());

    for (unsigned int i = 0; i < count; ++i)
    {
        const std::string& type = m_Textures[i].type;
        std::string number;

        if (type == "texture_diffuse")  number = std::to_string(diffuseIndex++);
        else if (type == "texture_specular") number = std::to_string(specularIndex++);
        else if (type == "texture_normal")   number = std::to_string(normalIndex++);

        // Set the sampler uniform to this texture unit index, then bind.
        // Texture::Bind activates the correct unit before binding.
        shader.setUniform1i(type + number, static_cast<int>(i));
        m_Textures[i].texture->Bind(i);
    }

    Renderer renderer;
    renderer.Draw(*m_VAO, *m_EBO);

    // Unbind each texture to avoid state pollution for subsequent draw calls.
    for (unsigned int i = 0; i < count; ++i)
        m_Textures[i].texture->Unbind();
}