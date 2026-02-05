#shader vertex // Specifies the vertex shader section
#version 330 core // Using GLSL version 3.30, compatible with OpenGL 3.3

// Vertex attributes matching our Vertex struct layout
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColour;
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
    vec2 grid = floor(uv * 10.0); // Increase the multiplier for smaller squares
    float check = mod(grid.x + grid.y, 2.0); // Alternate between 0 and 1
    vec3 color = mix(vec3(1.0), vec3(0.0), check); // Black and white
    FragColor = vec4(color, 1.0);
}
