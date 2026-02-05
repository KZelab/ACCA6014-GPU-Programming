#include "testProceduralArt.h"
#include "../Renderer.h"
#include "../vendor/imgui/imgui.h"

/*
 * ============================================================================
 * RENDERING PIPELINE FOR PROCEDURAL SHADERS
 * ============================================================================
 *
 * The rendering process for procedural shader art is elegantly simple:
 *
 *   1. FULLSCREEN QUAD: We render a simple rectangle covering the entire screen.
 *      This is our "canvas" - every pixel on screen corresponds to a fragment.
 *
 *   2. VERTEX SHADER: Minimal work - just passes vertex positions through.
 *      The quad vertices are in Normalized Device Coordinates (NDC): -1 to +1.
 *
 *   3. FRAGMENT SHADER: This is where the magic happens!
 *      For EVERY pixel, the GPU runs our shader code to compute its color.
 *      With millions of pixels and thousands of GPU cores, this is massively parallel.
 *
 *   4. UNIFORMS: We pass time and resolution so the shader can:
 *      - Animate (using time)
 *      - Calculate proper UV coordinates (using resolution)
 *
 * The CPU's job is minimal: just update uniforms and issue a single draw call.
 * The GPU does all the heavy lifting in parallel.
 * ============================================================================
 */

test::testProceduralArt::testProceduralArt(GLFWwindow* window)
    : m_Window(window)
{
    // Load both procedural shaders
    // These shaders generate visuals purely from math - no textures needed!
    m_PlasmaShader = std::make_unique<Shader>("res/Shaders/plasma.shader");
    m_NoiseShader = std::make_unique<Shader>("res/Shaders/noise.shader");

    // Create a fullscreen quad using the GeometryFactory
    // This quad spans from (-1,-1) to (1,1) in NDC - covering the entire viewport
    m_Quad = GeometryFactory::CreateFullscreenQuad();

    // Get window dimensions for UV coordinate calculation in shaders
    // Resolution is needed to convert gl_FragCoord (pixel position) to UV (0-1 range)
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    m_Resolution = glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

void test::testProceduralArt::Update(float deltaTime)
{
    // No update logic needed - all animation is driven by u_Time in the shader
    // The shader reads glfwGetTime() each frame for smooth animation
}

void test::testProceduralArt::Render()
{
    Renderer renderer;
    renderer.Clear();

    // Select active shader based on user choice
    Shader* shader = (m_CurrentShader == 0) ? m_PlasmaShader.get() : m_NoiseShader.get();

    shader->Bind();

    /*
     * UNIFORM UPDATES:
     * These values are sent to the GPU and remain constant for all fragments
     * in this draw call. Each pixel/fragment can read these same values.
     *
     * u_Time: Drives animation. By using sin(u_Time), cos(u_Time), etc.,
     *         shaders create smooth, cyclic animations.
     *
     * u_Resolution: Allows the shader to calculate normalized UV coordinates:
     *               uv = gl_FragCoord.xy / u_Resolution
     *               This gives us coordinates from (0,0) to (1,1)
     */
    float timeValue = static_cast<float>(glfwGetTime()) * m_TimeMultiplier;
    shader->setUniform1f("u_Time", timeValue);
    shader->setUniform2f("u_Resolution", m_Resolution.x, m_Resolution.y);

    // Draw the fullscreen quad - this triggers the fragment shader for every pixel
    m_Quad->Draw();
}

void test::testProceduralArt::RenderGUI()
{
    ImGui::Text("Procedural Shader Art");
    ImGui::Separator();

    // Shader selection
    const char* shaders[] = { "Plasma", "Noise" };
    ImGui::Combo("Shader", &m_CurrentShader, shaders, IM_ARRAYSIZE(shaders));

    // Brief description of current shader
    if (m_CurrentShader == 0) {
        ImGui::TextWrapped("Plasma: Overlapping sine waves create interference patterns. "
                          "Classic demoscene effect from the 1990s.");
    } else {
        ImGui::TextWrapped("Noise: Pseudo-random value noise using hash functions. "
                          "Foundation for procedural textures, terrain, clouds.");
    }

    ImGui::Separator();
    ImGui::SliderFloat("Time Multiplier", &m_TimeMultiplier, 0.1f, 5.0f);
    ImGui::Text("Resolution: %.0f x %.0f", m_Resolution.x, m_Resolution.y);
}
