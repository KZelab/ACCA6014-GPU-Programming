#pragma once
/*
 * ============================================================================
 * PROCEDURAL SHADER ART - Educational Demo
 * ============================================================================
 *
 * This test demonstrates PROCEDURAL GENERATION - creating visuals entirely
 * through mathematical functions rather than using pre-made textures or models.
 *
 * KEY CONCEPTS:
 * -------------
 * 1. FRAGMENT SHADERS: Programs that run on the GPU for every pixel on screen.
 *    With a 1920x1080 display, that's over 2 million parallel calculations per frame!
 *
 * 2. UNIFORMS: Variables passed from CPU (C++) to GPU (GLSL shader).
 *    - u_Time: Elapsed time for animation
 *    - u_Resolution: Screen dimensions for coordinate normalization
 *
 * 3. UV COORDINATES: Normalized screen position (0.0 to 1.0).
 *    These let shaders work independently of screen resolution.
 *
 * 4. PROCEDURAL PATTERNS: Mathematical functions that generate patterns:
 *    - Plasma: Overlapping sine/cosine waves create organic, flowing colors
 *    - Noise: Pseudo-random functions create natural-looking randomness
 *
 * WHY PROCEDURAL?
 * ---------------
 * - Infinite resolution (no pixelation when zooming)
 * - Tiny memory footprint (just math, no textures)
 * - Easy animation (just vary time parameter)
 * - Highly customizable through parameters
 *
 * PERFORMANCE NOTE:
 * -----------------
 * GPUs excel at this because they have thousands of cores designed for
 * parallel floating-point math. What would take seconds on a CPU happens
 * in milliseconds on a GPU.
 *
 * FURTHER READING:
 * ----------------
 * - The Book of Shaders: https://thebookofshaders.com/
 * - Shadertoy: https://www.shadertoy.com/ (thousands of examples)
 * - Inigo Quilez's articles: https://iquilezles.org/articles/
 * ============================================================================
 */

#include "Tests.h"
#include "../Shader.h"
#include "../Mesh/GeometryFactory.h"
#include <memory>
#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

namespace test {

    class testProceduralArt : public Tests {
    public:
        testProceduralArt(GLFWwindow* window);

        void Update(float deltaTime) override;
        void Render() override;
        void RenderGUI() override;

    private:
        GLFWwindow* m_Window;

        std::unique_ptr<Mesh> m_Quad;           // Fullscreen quad - our "canvas"

        std::unique_ptr<Shader> m_PlasmaShader; // Sine/cosine wave interference pattern
        std::unique_ptr<Shader> m_NoiseShader;  // Pseudo-random value noise		
        std::unique_ptr<Shader> m_CircleRipples;
        std::unique_ptr<Shader> m_Checkerboard;
        std::unique_ptr<Shader> m_FBMNoise;


        // Uniforms sent to GPU each frame
        glm::vec2 m_Resolution;       // Screen dimensions for UV calculation
        float m_TimeMultiplier = 1.0f; // Speed control for animations
		int m_CurrentShader = 0;       // 0: Plasma, 1: Noise, 2: Circle Ripples, 3: Checkerboard, 4: FBM Noise
    };

} // namespace test
