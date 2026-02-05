#shader vertex // Specifies the vertex shader section
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 fragColor;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragColor = aColor; // Pass the color to the fragment shader
}


#shader fragment // Specifies the fragment shader section
#version 330 core

in vec4 fragColor;

uniform vec4 u_Color; // The colour passed in from the CPU-side code

out vec4 color;

void main() {
    color =u_Color; // Use the provided uniform color
}

