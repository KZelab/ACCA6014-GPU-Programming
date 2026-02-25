#pragma once
#include "Tests.h"
#include "../Renderer.h"
#include "../Shader.h"
#include "../ComputeShader.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

#include <memory>

struct GLFWwindow;

namespace test
{
    class TestGPUParticles : public Tests
    {
    public:
        TestGPUParticles(GLFWwindow* window);
        ~TestGPUParticles();

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        void RebuildSSBO();

        static const unsigned int MAX_PARTICLES = 1000000;
        static const unsigned int WORK_GROUP_SIZE = 256;

        // OpenGL objects
        unsigned int m_SSBO;
        unsigned int m_VAO;
        std::unique_ptr<ComputeShader> m_ComputeShader;
        std::unique_ptr<Shader> m_RenderShader;

        // Matrices
        glm::mat4 m_Proj;
        glm::mat4 m_View;

        // Settings
        glm::vec2 m_EmitterPos;
        float m_Gravity;
        float m_EmissionRate;
        float m_SpeedMin;
        float m_SpeedMax;
        float m_LifeMin;
        float m_LifeMax;
        float m_SizeMin;
        float m_SizeMax;
        int m_MaxParticles;
        glm::vec4 m_ColourStart;
        glm::vec4 m_ColourEnd;
        float m_Time;

        // Performance tracking
        unsigned int m_QueryCompute[2];  // double-buffered GL timer queries
        unsigned int m_QueryRender[2];
        int m_QueryBack;                 // index of the query we're reading results from
        float m_ComputeTimeMs;
        float m_RenderTimeMs;

        static const int FRAME_HISTORY_SIZE = 120;
        float m_FrameTimeHistory[FRAME_HISTORY_SIZE];
        float m_ComputeTimeHistory[FRAME_HISTORY_SIZE];
        float m_RenderTimeHistory[FRAME_HISTORY_SIZE];
        int m_FrameHistoryIdx;
    };
}
