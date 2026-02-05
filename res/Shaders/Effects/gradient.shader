#shader vertex
#version 330 core

// Vertex attributes - position and texture coordinates
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;    // Not used in this shader, but part of Vertex struct
layout(location = 2) in vec3 aColour;    // Not used in this shader, but part of Vertex struct
layout(location = 3) in vec2 aTexCoord;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoord;
    // Pass position directly - quad is already in NDC (-1 to +1)
    gl_Position = vec4(aPosition, 1.0);
}


#shader fragment
#version 330

out vec4 FragColor;

in vec2 TexCoords;

uniform float uTime;

uniform vec2 uResolution;

void main()
{

    vec2 uv = TexCoords * uResolution;
    float colourShift = sin(uTime) * 0.5 + 0.5;
    FragColor = vec4(uv / uResolution, colourShift, 1.0); // Colour output based on texture coordinates
}