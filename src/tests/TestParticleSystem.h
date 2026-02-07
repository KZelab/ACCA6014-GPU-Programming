#pragma once
#include "Tests.h"
#include "../Renderer.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../VertexArray.h"
#include "../Shader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

#include <memory>
#include <vector>

struct GLFWwindow;

namespace test
{
    struct Particle
    {
        glm::vec2 pos;
        glm::vec2 vel;
        glm::vec4 color;
        float life;
        float maxLife;
        float size;
    };

    class TestParticleSystem : public Tests
    {
    public:
        TestParticleSystem(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        void EmitParticles(float dt);
        void RebuildVertexData();

        static const unsigned int MAX_PARTICLES = 10000;

        // Particles
        std::vector<Particle> m_Particles;
        unsigned int m_ActiveCount;

        // Vertex data: 4 verts per particle, each vert = 2 pos + 4 color = 6 floats
        std::vector<float> m_VertexData;

        // OpenGL objects
        std::unique_ptr<VertexArray> m_VAO;
        std::unique_ptr<VertexBuffer> m_VBO;
        std::unique_ptr<IndexBuffer> m_IBO;
        std::unique_ptr<Shader> m_Shader;

        // Matrices
        glm::mat4 m_Proj;
        glm::mat4 m_View;

        // Emitter settings
        glm::vec2 m_EmitterPos;
        float m_Gravity;
        float m_EmissionRate;    // particles per second
        float m_EmissionAccum;   // accumulator for fractional emissions
        float m_SpeedMin;
        float m_SpeedMax;
        float m_LifeMin;
        float m_LifeMax;
        float m_SizeMin;
        float m_SizeMax;
        int m_MaxParticles;      // user-adjustable cap (up to MAX_PARTICLES)
        glm::vec4 m_ColorStart;
        glm::vec4 m_ColorEnd;
    };
}
