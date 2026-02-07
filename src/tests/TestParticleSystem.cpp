#include "TestParticleSystem.h"
#include <cstdlib>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace test
{
    // Helper: random float in [lo, hi]
    static float RandRange(float lo, float hi)
    {
        float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        return lo + t * (hi - lo);
    }

    TestParticleSystem::TestParticleSystem(GLFWwindow* /*window*/)
        : m_ActiveCount(0)
        , m_EmitterPos(480.0f, 300.0f)
        , m_Gravity(-200.0f)
        , m_EmissionRate(500.0f)
        , m_EmissionAccum(0.0f)
        , m_SpeedMin(50.0f)
        , m_SpeedMax(200.0f)
        , m_LifeMin(1.0f)
        , m_LifeMax(3.0f)
        , m_SizeMin(2.0f)
        , m_SizeMax(6.0f)
        , m_MaxParticles(5000)
        , m_ColorStart(1.0f, 0.6f, 0.1f, 1.0f)
        , m_ColorEnd(1.0f, 0.0f, 0.0f, 0.0f)
    {
        srand(42);

        // Reserve space
        m_Particles.resize(MAX_PARTICLES);
        // All particles start dead (life <= 0)
        for (auto& p : m_Particles)
            p.life = 0.0f;

        // 4 verts per particle, 6 floats per vert (2 pos + 4 color)
        m_VertexData.resize(MAX_PARTICLES * 4 * 6, 0.0f);

        // --- Build static index buffer for all quads ---
        // Each particle is a quad: 4 verts, 6 indices (two triangles)
        std::vector<unsigned int> indices(MAX_PARTICLES * 6);
        for (unsigned int i = 0; i < MAX_PARTICLES; i++)
        {
            unsigned int base = i * 4;
            unsigned int idx = i * 6;
            indices[idx + 0] = base + 0;
            indices[idx + 1] = base + 1;
            indices[idx + 2] = base + 2;
            indices[idx + 3] = base + 2;
            indices[idx + 4] = base + 3;
            indices[idx + 5] = base + 0;
        }

        // Create OpenGL objects
        m_VAO = std::make_unique<VertexArray>();
        m_VBO = std::make_unique<VertexBuffer>(MAX_PARTICLES * 4 * 6 * sizeof(float)); // dynamic
        m_IBO = std::make_unique<IndexBuffer>(indices.data(), MAX_PARTICLES * 6);

        VertexBufferLayout layout;
        layout.Push<float>(2); // position
        layout.Push<float>(4); // color (rgba)
        m_VAO->AddBuffer(*m_VBO, layout);

        m_Shader = std::make_unique<Shader>(R"(res/Shaders/ParticleShader.shader)");

        // Orthographic projection matching the window
        m_Proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
        m_View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));

        // Unbind everything
        m_VAO->unBind();
        m_VBO->Unbind();
        m_IBO->Unbind();
        m_Shader->Unbind();
    }

    void TestParticleSystem::Update(float deltaTime)
    {
        // Clamp large dt to avoid explosion after pauses
        if (deltaTime > 0.1f) deltaTime = 0.1f;

        // Emit new particles
        EmitParticles(deltaTime);

        // Update existing particles
        m_ActiveCount = 0;
        for (unsigned int i = 0; i < MAX_PARTICLES; i++)
        {
            Particle& p = m_Particles[i];
            if (p.life <= 0.0f) continue;

            p.life -= deltaTime;
            if (p.life <= 0.0f) continue;

            // Physics
            p.vel.y += m_Gravity * deltaTime;
            p.pos += p.vel * deltaTime;

            m_ActiveCount++;
        }

        // Rebuild vertex data and upload to GPU
        RebuildVertexData();
    }

    void TestParticleSystem::EmitParticles(float dt)
    {
        m_EmissionAccum += m_EmissionRate * dt;
        int toEmit = static_cast<int>(m_EmissionAccum);
        m_EmissionAccum -= static_cast<float>(toEmit);

        for (int e = 0; e < toEmit; e++)
        {
            // Find a dead particle slot
            int slot = -1;
            for (unsigned int i = 0; i < static_cast<unsigned int>(m_MaxParticles); i++)
            {
                if (m_Particles[i].life <= 0.0f)
                {
                    slot = static_cast<int>(i);
                    break;
                }
            }
            if (slot < 0) break; // All slots full

            Particle& p = m_Particles[slot];
            p.pos = m_EmitterPos;
            p.life = RandRange(m_LifeMin, m_LifeMax);
            p.maxLife = p.life;
            p.size = RandRange(m_SizeMin, m_SizeMax);

            // Random direction, random speed
            float angle = RandRange(0.0f, 2.0f * static_cast<float>(M_PI));
            float speed = RandRange(m_SpeedMin, m_SpeedMax);
            p.vel = glm::vec2(cosf(angle), sinf(angle)) * speed;

            // Random color between start and end
            float t = RandRange(0.0f, 1.0f);
            p.color = glm::mix(m_ColorStart, m_ColorEnd, t);
        }
    }

    void TestParticleSystem::RebuildVertexData()
    {
        // Write 4 verts per alive particle, 6 floats each (pos.x, pos.y, r, g, b, a)
        for (unsigned int i = 0; i < MAX_PARTICLES; i++)
        {
            const Particle& p = m_Particles[i];
            if (p.life <= 0.0f)
            {
                // Write degenerate (zero-area) quad so indices still line up
                for (int v = 0; v < 4; v++)
                {
                    unsigned int base = i * 4 * 6 + v * 6;
                    m_VertexData[base + 0] = 0.0f;
                    m_VertexData[base + 1] = 0.0f;
                    m_VertexData[base + 2] = 0.0f;
                    m_VertexData[base + 3] = 0.0f;
                    m_VertexData[base + 4] = 0.0f;
                    m_VertexData[base + 5] = 0.0f;
                }
                continue;
            }

            // Fade alpha based on remaining life
            float lifeRatio = p.life / p.maxLife;
            float alpha = p.color.a * lifeRatio;
            float hs = p.size * 0.5f;

            // 4 corners of quad: BL, BR, TR, TL
            float verts[4][2] = {
                { p.pos.x - hs, p.pos.y - hs },
                { p.pos.x + hs, p.pos.y - hs },
                { p.pos.x + hs, p.pos.y + hs },
                { p.pos.x - hs, p.pos.y + hs },
            };

            for (int v = 0; v < 4; v++)
            {
                unsigned int base = i * 4 * 6 + v * 6;
                m_VertexData[base + 0] = verts[v][0];
                m_VertexData[base + 1] = verts[v][1];
                m_VertexData[base + 2] = p.color.r;
                m_VertexData[base + 3] = p.color.g;
                m_VertexData[base + 4] = p.color.b;
                m_VertexData[base + 5] = alpha;
            }
        }

        // Upload to GPU
        m_VBO->Update(m_VertexData.data(),
            static_cast<unsigned int>(m_VertexData.size() * sizeof(float)));
    }

    void TestParticleSystem::Render()
    {
        GlCall(glEnable(GL_BLEND));
        GlCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        Renderer renderer;

        glm::mat4 mvp = m_Proj * m_View;
        m_Shader->Bind();
        m_Shader->setUniformMat4f("u_MVP", mvp);

        renderer.Draw(*m_VAO, *m_IBO, *m_Shader);

        GlCall(glDisable(GL_BLEND));
    }

    void TestParticleSystem::RenderGUI()
    {
        ImGui::Text("Active Particles: %u", m_ActiveCount);

        float rate = ImGui::GetIO().Framerate;
        ImGui::Text("%.1f FPS (%.3f ms/frame)", rate, 1000.0f / rate);

        ImGui::Separator();
        ImGui::Text("Emitter Settings");

        ImGui::SliderFloat2("Emitter Pos", &m_EmitterPos.x, 0.0f, 960.0f);
        ImGui::SliderInt("Max Particles", &m_MaxParticles, 100, static_cast<int>(MAX_PARTICLES));
        ImGui::SliderFloat("Emission Rate", &m_EmissionRate, 10.0f, 5000.0f);
        ImGui::SliderFloat("Gravity", &m_Gravity, -500.0f, 500.0f);

        ImGui::Separator();
        ImGui::Text("Particle Properties");

        ImGui::SliderFloat("Speed Min", &m_SpeedMin, 0.0f, 500.0f);
        ImGui::SliderFloat("Speed Max", &m_SpeedMax, 0.0f, 500.0f);
        ImGui::SliderFloat("Life Min", &m_LifeMin, 0.1f, 10.0f);
        ImGui::SliderFloat("Life Max", &m_LifeMax, 0.1f, 10.0f);
        ImGui::SliderFloat("Size Min", &m_SizeMin, 1.0f, 20.0f);
        ImGui::SliderFloat("Size Max", &m_SizeMax, 1.0f, 20.0f);

        ImGui::Separator();
        ImGui::Text("Colors");
        ImGui::ColorEdit4("Color Start", &m_ColorStart.x);
        ImGui::ColorEdit4("Color End", &m_ColorEnd.x);
    }
}
