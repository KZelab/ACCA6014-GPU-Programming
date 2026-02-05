#shader vertex
#version 330 core

// Vertex attributes matching our Vertex struct layout
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;    // Not used, but part of layout
layout(location = 2) in vec3 aColour;    // Not used, but part of layout
layout(location = 3) in vec2 aTexCoord;


void main() {
    gl_Position = vec4(aPosition, 1.0);
}


#shader fragment // Specifies the fragment shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

out vec4 FragColor;

uniform float u_Time;
uniform vec2 u_Resolution;


void main() {
    vec2 uv = gl_FragCoord.xy / u_Resolution;
    uv -= 0.5; // Centre the coordinates
    float dist = length(uv); // Distance from the centre

    // Ripple effect
    float wave = 0.5 + 0.5 * sin(10.0 * dist - u_Time * 5.0);
    wave = smoothstep(0.4, 0.6, wave); // Smooth the edges
    vec3 color = vec3(wave);

    FragColor = vec4(color, 1.0);
}
