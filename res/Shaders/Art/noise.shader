/*
 * ============================================================================
 * VALUE NOISE SHADER 
 * ============================================================================
 *
 * WHAT IS NOISE?
 * --------------
 * In graphics, "noise" refers to pseudo-random functions that produce smooth,
 * continuous random values. Unlike true randomness (which is jagged), noise
 * functions create organic-looking patterns perfect for natural phenomena.
 *
 * TYPES OF NOISE:
 * ---------------
 * 1. VALUE NOISE (this shader): Random values at grid points, interpolated
 * 2. PERLIN NOISE: Random gradients at grid points (smoother)
 * 3. SIMPLEX NOISE: Optimized version by Ken Perlin (faster in higher dimensions)
 * 4. WORLEY/CELLULAR NOISE: Distance to random points (creates cell patterns)
 *
 * APPLICATIONS:
 * -------------
 * - Terrain generation (heightmaps)
 * - Cloud and smoke rendering
 * - Water/ocean surfaces
 * - Procedural textures (wood grain, marble, etc.)
 * - Film grain and artistic effects
 * - Game world generation (Minecraft uses noise!)
 *
 * THE ALGORITHM:
 * --------------
 * Value noise works in three steps:
 *
 * 1. HASH FUNCTION: Convert 2D coordinates to pseudo-random values.
 *    We use the classic: fract(sin(dot(p, magic_numbers)) * big_number)
 *    This is deterministic - same input always gives same output.
 *
 * 2. GRID SAMPLING: For any point, find the 4 surrounding grid corners
 *    and get their random values (a, b, c, d).
 *
 * 3. INTERPOLATION: Smoothly blend between corner values based on
 *    position within the grid cell. We use smoothstep for smooth curves.
 *
 * SMOOTHSTEP EXPLAINED:
 * ---------------------
 * smoothstep(t) = t * t * (3 - 2 * t)
 *
 * This S-curve function:
 * - Starts slow (derivative = 0 at t=0)
 * - Speeds up in the middle
 * - Slows down at the end (derivative = 0 at t=1)
 *
 * This eliminates visible grid artifacts that linear interpolation would create.
 *
 * EXPERIMENT IDEAS:
 * -----------------
 * - Layer multiple octaves (add noise at different scales) for more detail
 * - Use noise to distort other patterns
 * - Map noise to colours instead of greyscale
 * - Combine with plasma for psychedelic effects
 * ============================================================================
 */

#shader vertex
#version 330 core

// Vertex attributes matching our Vertex struct layout
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColour;
layout(location = 3) in vec2 aTexCoord;


void main() {
    gl_Position = vec4(aPosition, 1.0);
}


#shader fragment
#version 330 core

out vec4 FragColor;

uniform float u_Time;
uniform vec2 u_Resolution;

/*
 * HASH FUNCTION
 * -------------
 * Converts a 2D point to a pseudo-random value between 0 and 1.
 *
 * How it works:
 * 1. dot(p, vec2(127.1, 311.7)) - Projects point onto a line with "magic" slope
 * 2. sin(...) - Wraps the result into [-1, 1] with high-frequency oscillation
 * 3. * 43758.5453123 - Amplifies to create more variation in the fractional part
 * 4. fract(...) - Takes only the fractional part [0, 1)
 *
 * The "magic numbers" are chosen to minimize visible patterns. Different
 * numbers work, but these are well-tested in the graphics community.
 */
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

/*
 * VALUE NOISE FUNCTION
 * --------------------
 * Produces smooth, continuous random values across 2D space.
 *
 * The algorithm:
 * 1. Find which grid cell we're in: i = floor(p)
 * 2. Find position within that cell: f = fract(p), ranges [0,1]
 * 3. Sample random values at all 4 corners (a, b, c, d)
 * 4. Smoothly interpolate based on position within cell
 *
 * Grid visualization:
 *
 *     c -------- d
 *     |    .p    |     p = point we're evaluating
 *     |          |     a,b,c,d = corner values
 *     a -------- b
 *
 * Interpolation uses smoothstep (Hermite interpolation) for smooth results.
 */
float noise(vec2 p) {
    // Integer part: which grid cell are we in?
    vec2 i = floor(p);

    // Fractional part: where within the cell? (0.0 to 1.0)
    vec2 f = fract(p);

    // Get random values at the four corners of this grid cell
    float a = hash(i);                    // Bottom-left  (0,0)
    float b = hash(i + vec2(1.0, 0.0));   // Bottom-right (1,0)
    float c = hash(i + vec2(0.0, 1.0));   // Top-left     (0,1)
    float d = hash(i + vec2(1.0, 1.0));   // Top-right    (1,1)

    // Smoothstep interpolation weights
    // f * f * (3.0 - 2.0 * f) is the smoothstep formula
    // This creates an S-curve that starts and ends with zero derivative
    vec2 u = f * f * (3.0 - 2.0 * f);

    // Bilinear interpolation with smoothstep weights
    // First interpolate along x (bottom edge: a->b, top edge: c->d)
    // Then interpolate along y between those results
    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main() {
    // Calculate UV coordinates (0 to 1 across screen)
    vec2 uv = gl_FragCoord.xy / u_Resolution;

    // Scale up the UV coordinates to see more noise detail
    // Higher scale = more "zoomed out", showing more variation
    uv *= 10.0;

    // Add time to create animation
    // The noise function is deterministic, so adding time to input
    // creates the illusion of flowing/moving noise
    float n = noise(uv + u_Time);

    // Output as greyscale
    // Try: vec3(n, n*0.8, n*0.6) for sepia-toned noise
    vec3 colour = vec3(n);
    FragColor = vec4(colour, 1.0);
}
