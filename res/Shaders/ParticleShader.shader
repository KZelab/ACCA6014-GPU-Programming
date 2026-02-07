#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

out vec4 v_Color;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(position, 0.0, 1.0);
    v_Color = color;
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 colour;

in vec4 v_Color;

void main()
{
    colour = v_Color;
}
