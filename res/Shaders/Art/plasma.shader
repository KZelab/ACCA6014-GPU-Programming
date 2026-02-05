/*
 * ============================================================================
 * PLASMA SHADER
 * ============================================================================
 *
 * HISTORY:
 * --------
 * The plasma effect originated in the demoscene of the late 1980s/early 1990s.
 * Programmers created mesmerizing visuals on limited hardware by exploiting
 * mathematical properties of trigonometric functions.
 *
 * THE MATHEMATICS:
 * ----------------
 * Plasma works by combining multiple sine/cosine waves at different frequencies
 * and phases. When waves overlap, they create INTERFERENCE PATTERNS:
 *
 *   - Where waves align (constructive interference): bright regions
 *   - Where waves cancel (destructive interference): dark regions
 *
 * The key insight: sin() and cos() are PERIODIC functions, meaning they
 * repeat forever. By combining them cleverly, we get complex, non-repeating
 * patterns that seem organic and flowing.
 *
 * FORMULA BREAKDOWN:
 * ------------------
 *   sin(time + distance * frequency)
 *
 *   - time: Makes the pattern animate (phase shift over time)
 *   - distance: Creates spatial variation (rings emanating from center)
 *   - frequency: Controls how "tight" the waves are (higher = more ripples)
 *
 * By combining multiple such terms with different inputs (distance, x, y),
 * we create rich interference patterns.
 *
 * COLOUR MAPPING:
 * ---------------
 * The combined wave values are normalized to 0-1 range, then used as
 * greyscale intensity. For colour, you could map to a palette or use
 * the value differently for R, G, B channels.
 *
 * EXPERIMENT IDEAS:
 * -----------------
 * - Change frequencies (the * 10.0 values) for different pattern scales
 * - Add more sin/cos terms for complexity
 * - Use different color mappings (try: vec3(sin(color), cos(color), 1.0-color))
 * - Replace length(uv) with other distance functions
 * ============================================================================
 */

#shader vertex
#version 330 core

// Vertex attributes - position and texture coordinates
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;  
layout(location = 2) in vec3 aColour; 
layout(location = 3) in vec2 aTexCoord;

void main() {
    // Pass position directly - quad is already in NDC (-1 to +1)
    gl_Position = vec4(aPosition, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;

// Uniforms passed from C++ code each frame
uniform float u_Time;       // Elapsed time in seconds - drives animation
uniform vec2 u_Resolution;  // Screen dimensions in pixels

void main() {
    // ========================================================================
    // STEP 1: Calculate UV coordinates (normalized screen position)
    // ========================================================================
    // gl_FragCoord.xy gives us the pixel position (e.g., 0-1920, 0-1080)
    // Dividing by resolution normalizes to 0.0 - 1.0 range
    vec2 uv = gl_FragCoord.xy / u_Resolution;

    // Centre the coordinates so (0,0) is at screen center instead of corner
    // After this: uv ranges from -1.0 to +1.0
    uv -= 0.5;    // Shift from [0,1] to [-0.5, 0.5]
    uv *= 2.0;    // Scale from [-0.5, 0.5] to [-1, 1]

    // ========================================================================
    // STEP 2: Generate plasma pattern using wave interference
    // ========================================================================
    // Each term creates a different wave pattern:

    // Term 1: Circular ripples from center (based on distance from origin)
    // length(uv) = sqrt(uv.x² + uv.y²) = distance from center
    float wave1 = 0.5 + 0.5 * sin(u_Time + length(uv) * 10.0);

    // Term 2: Vertical waves (based on x coordinate only)
    float wave2 = 0.5 + 0.5 * cos(u_Time + uv.x * 10.0);

    // Term 3: Horizontal waves (based on y coordinate only)
    float wave3 = 0.5 + 0.5 * sin(u_Time + uv.y * 10.0);

    // Combine all waves and average them
    // The 0.5 + 0.5 * sin(...) maps sin's [-1,1] output to [0,1]
    float color = (wave1 + wave2 + wave3) / 3.0;

    // ========================================================================
    // STEP 3: Output final color
    // ========================================================================
    // Using greyscale (same value for R, G, B)
    // Try: vec3(color, color * 0.5, 1.0 - color) for a colour gradient!
    FragColor = vec4(vec3(color), 1.0);
}
