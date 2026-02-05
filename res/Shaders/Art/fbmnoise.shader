


//Fractal Brownian Motion (fBm) is a fractional extension of classical Brownian motion, 
//characterized by its self-similarity and non-stationary stochastic nature. 
//In the context of computer graphics and procedural generation, 
//fBm is utilized to construct heterogeneous textures by summing successive layers—or "octaves"—of a noise basis function. 
//Each octave represents a higher frequency and lower amplitude than its predecessor, 
//effectively simulating the multi-scale structural detail found in natural phenomena such as terrain elevation 
//and atmospheric turbulence [1].The mathematical formulation of a discretized fBm function can be expressed as 
//the summation of $n$ octaves of a noise function 
//$N$:$$f(\mathbf{p}) = \sum_{i=0}^{n-1} G^i N(L^i \mathbf{p})$$
//In this expression, $G$ represents the gain (or persistence), 
//which controls the rate of amplitude decay, and $L$ represents the lacunarity, 
//which determines the frequency gap between successive layers.
//Standard implementations typically utilize a gain of $0.5$ and a lacunarity of $2.0$, 
//resulting in a power spectrum that follows a $1/f$ distribution [2].

#shader vertex
#version 330 core
/**
 * Vertex Stage: Performs coordinate transformation.
 * Here, it maps model-space attributes to clip-space coordinates.
 */
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;    
layout(location = 2) in vec3 aColour;    
layout(location = 3) in vec2 aTexCoord;

void main() {
    // Convert 3D position to 4D clip space; Z is usually 0 for 2D noise planes
    gl_Position = vec4(aPosition, 0.0); //try 0 and 1
}

#shader fragment
#version 330
out vec4 FragColor;
uniform float uTime;
uniform vec2 uResolution;

/**
 * Stochastic Hash Function: Maps a 2D vector to a deterministic 
 * pseudo-random scalar using a high-frequency trigonometric function.
 */
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
};

/**
 * Interpolated Value Noise: Computes a smooth scalar field by 
 * interpolating between random values defined at integer grid points.
 */
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Quintic interpolation curve: 6t^5 - 15t^4 + 10t^3.
    // Provides C2 continuity, eliminating visible artifacts in derivatives.
    vec2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10); 
    
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
};

/**
 * fBm Spectral Synthesis: Iteratively accumulates weighted noise octaves.
 */
float fbm(vec2 uv) {
    float total = 0.0;
    float amplitude = 0.5; // Initial weighting
    float frequency = 1.0; // Initial spatial scale

    for (int i = 0; i < 5; i++) {
        // Accumulate the noise contribution for the current octave
        total += noise(uv * frequency) * amplitude;

        // Apply geometric progression to frequency (lacunarity) 
        // and amplitude (persistence)
        frequency *= 2.0;
        amplitude *= 0.5;
    }
    return total;
}

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;
    uv *= 10.0; // Global spatial scaling

    // Sample the fBm signal with a temporal offset for dynamic animation
    float n = fbm(uv + uTime);

    // Represent the scalar noise value as a normalized luminance vector
    FragColor = vec4(vec3(n), 1.0);
};