#pragma once
#include <memory>
#include "glm/glm.hpp"
#include "../Shader.h"
#include "../Mesh/Mesh.h"

namespace test
{
    class DefaultScene
    {
    public:
        DefaultScene();
        ~DefaultScene();

        DefaultScene(const DefaultScene&) = delete;
        DefaultScene& operator=(const DefaultScene&) = delete;

        // Call as first statement in child's Render().
        // Handles glClearColor + glClear internally — don't also call renderer.Clear().
        void Render(const glm::mat4& view,
                    const glm::mat4& projection,
                    float tileSize = 1.0f);

    private:
        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Mesh>   m_FloorMesh;
    };
}
